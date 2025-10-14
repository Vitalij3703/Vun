#pragma once
#include "parser.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include "ast.hpp"
#include "err.hpp"
#include <unordered_map>
#include <optional>
#include <sstream>

const std::string OPS[4] = {"+", "-", "*", "/"};

bool debug = false;

// parse binary expression
float pbexpr(ast::n* expr){
    if (!(expr->type == "binaryop")) throw ParseError("pbexpr called on non-binary node");
    ast::n* left = expr->children[0].get();
    ast::n* right = expr->children[1].get();
    std::string op = expr->value;

    auto evalNodeToFloat = [&](ast::n* node) -> float {
        if (!node) throw ParseError("Null node in expression");
        if (node->type == "literal") {
            try {
                if (node->is_int_lit()) {
                    return static_cast<float>(std::stoll(node->value));
                } else {
                    return std::stof(node->value);
                }
            } catch (...) {
                throw ParseError(std::string("Invalid numeric literal: ") + node->value);
            }
        } else if (node->type == "binaryop") {
            return pbexpr(node);
        } else if (node->type == "ref") {
            throw ParseError("Working on this!");
        } else {
            throw ParseError("Unsupported node type in numeric expression: " + node->type);
        }
    };

    float l = evalNodeToFloat(left);
    float r = evalNodeToFloat(right);

    if (op == "+") return l + r;
    if (op == "-") return l - r;
    if (op == "*") return l * r;
    if (op == "/") {
        if (r == 0.0f) throw RuntimeError("Division by zero in binary expression");
        return l / r;
    }
    throw ParseError("Invalid binary operator: " + op);
}




class runtime {
    std::vector<std::unique_ptr<ast::n>> nodesl;
    std::unordered_map<std::string, std::string> vars;   // stores variables
    std::unordered_map<std::string, ast::n*> functions; // stores functions

public:
    runtime(const std::string& in) {
        parser p(in);
        if (debug) std::cout << "[DEBUG] input size: " << in.size() << "\n";
        nodesl = p.parse();
        if (debug) std::cout << "[DEBUG] parsed nodes count: " << nodesl.size() << "\n";

        auto top_ret = run(nodesl);
        if (top_ret) {
            // top-level return not allowed
            throw RuntimeError("Return at top-level is not allowed");
        }
    }

private:
    std::string evaluateExpression(ast::n* node) {
        if (!node) throw ParseError("Null node in eval");

        if (node->type == "literal") {
            return node->value;
        }

        if (node->type == "binaryop") {
            float f = pbexpr(node);
            std::ostringstream oss;
            oss << f;
            return oss.str();
        }

        if (node->type == "ref") {
            auto it = vars.find(node->value);
            if (it == vars.end()) throw RuntimeError("Unknown variable: " + node->value);
            return it->second;
        }

        if (node->type == "call") {
            std::vector<std::string> arg_values;
            for (const auto& child : node->children) {
                arg_values.push_back(evaluateExpression(child.get()));
            }

            const std::string call_name = node->value;
            auto it_fn = functions.find(call_name);
            if (it_fn != functions.end()) {
                ast::n* fn_node = it_fn->second;
                auto params_ptr = fn_node->get_params();
                if (!params_ptr) throw RuntimeError("Function has no params container");
                auto& params = *params_ptr;

                if (params.size() != arg_values.size()) {
                    throw RuntimeError("Argument count mismatch when calling function '" + call_name + "'");
                }

                auto old_vars = vars; // save old vars so scope works
                for (size_t i = 0; i < params.size(); ++i) {
                    const std::string& param_name = params[i].second;
                    vars.insert_or_assign(param_name, arg_values[i]);
                }
                auto ret = run(fn_node->children);
                vars = std::move(old_vars);

                if (ret) {
                    return *ret;
                } else {
                    // if function returned nothing (type is void): return void
                    return std::string{};
                }
            }
            return std::string{};
        }

        throw RuntimeError("Unsupported node type in expression: " + node->type);
    }

public:
    std::optional<std::string> run(const std::vector<std::unique_ptr<ast::n>>& nodes) {
        if (nodes.empty()) {
            if (debug) std::cout << "[DEBUG] input empty\n";
            return std::nullopt;
        }

        for (const auto& node : nodes) {
            if (!node) {
                if (debug) std::cout << "[DEBUG] null node\n";
                continue;
            }
            if (debug) std::cout << "[DEBUG] node.type: \"" << node->type << "\" node.value: \"" << node->value << "\"\n";

            if (node->type == "var") {
                std::string varname = node->value;
                if (!node->children.empty() && node->children[0]) {
                    // allow initializer to be any expression now
                    std::string assigned = evaluateExpression(node->children[0].get());
                    vars.insert_or_assign(varname, assigned);
                    if (debug) std::cout << "[DEBUG] assigned var '" << varname << "' with the value \"" << assigned << "\"\n";
                } else {
                    throw RuntimeError("Variable \"" + varname + "\" declared without initializer");
                }
                continue;
            }

            if (node->type == "literal") {
                if (debug) std::cout << "[DEBUG] literal value: \"" << node->value << "\"\n";
                continue;
            }

            if (node->type == "function") {
                if (debug) std::cout << "[DEBUG] processing function: \"" << node->value << "\"\n";
                functions.insert_or_assign(node->value, node.get());
                continue;
            }

            if (node->type == "return") {
                if (node->children.empty() || !node->children[0]) {
                    // void return (empty string)
                    return std::make_optional(std::string{});
                } else {
                    std::string ret = evaluateExpression(node->children[0].get());
                    if (debug) std::cout << "[DEBUG] return value: " << ret << "\n";
                    return std::make_optional(ret);
                }
            }

            if (node->type == "call") {
                evaluateExpression(node.get());
                std::vector<std::string> arg_values;
                for (const auto& child : node->children) arg_values.push_back(evaluateExpression(child.get()));


                const std::string call_name = node->value;

                // print values of args to default stream (stdout)
                if (call_name == "IOds_print") {
                    for (const auto& v : arg_values) std::cout << v;
                    continue;
                }

                // same as builtin above but adds a newline at the end
                if (call_name == "IOds_println") {
                    std::string result;
                    for (const auto& v : arg_values) result+=v;
                    std::cout << result<<"\n";
                    continue;
                }

                // prints values of args to error stream (stderr)
                if (call_name == "IOes_print") {
                    std::string result;
                    for (const auto& v : arg_values) result+=v;
                    std::cerr << result;
                    continue;
                }

                // same as builtin above but adds a newline at the end
                if (call_name == "IOes_println") {
                    std::string result;
                    for (const auto& v : arg_values) result+=v;
                    std::cerr << result<< "\n";
                    continue;
                }

                // toggles debug 
                if (call_name == "tdebug") {
                    debug = !debug;
                    continue;
                }
                continue;
            }
        }

        return std::nullopt; // no return encountered
    }
}; // end runtime