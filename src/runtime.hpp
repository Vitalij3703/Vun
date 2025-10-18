// i MAY be using the std too much
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
#include <stdexcept>
#include <variant>
#include <cstdint>


const std::string OPS[4] = {"+", "-", "*", "/"};

bool debug = false;

enum class ValType { INT, FLOAT, STR, BOOL, VOID };

struct Value {
    std::variant<std::int64_t, double, std::string, bool, std::monostate> v;
    ValType tag; // the value type

    Value() : v(std::monostate{}), tag(ValType::VOID) {}
    Value(std::int64_t i) : v(i), tag(ValType::INT) {}
    Value(double f) : v(f), tag(ValType::FLOAT) {}
    Value(const std::string& s) : v(s), tag(ValType::STR) {}
    Value(std::string&& s) : v(std::move(s)), tag(ValType::STR) {}
    Value(const char* s) : v(std::string(s)), tag(ValType::STR) {}
    Value(bool b) : v(b), tag(ValType::BOOL) {}
    static Value Void() { return Value(); }

    bool is_int() const   { return tag == ValType::INT; }
    bool is_float() const { return tag == ValType::FLOAT; }
    bool is_str() const   { return tag == ValType::STR; }
    bool is_bool() const  { return tag == ValType::BOOL; }
    bool is_void() const  { return tag == ValType::VOID; }

    std::int64_t as_int() const {
        if (is_int()) return std::get<std::int64_t>(v);
        if (is_float()) return static_cast<std::int64_t>(std::get<double>(v));
        if (is_bool()) return std::get<bool>(v) ? 1 : 0;
        throw std::runtime_error("Value is not convertible to int");
    }
    double as_float() const {
        if (is_float()) return std::get<double>(v);
        if (is_int()) return static_cast<double>(std::get<std::int64_t>(v));
        if (is_bool()) return std::get<bool>(v) ? 1.0 : 0.0;
        throw std::runtime_error("Value is not convertible to float");
    }
    const std::string& as_str() const {
        if (is_str()) return std::get<std::string>(v);
        throw std::runtime_error("Value is not a string");
    }
    bool as_bool() const {
        if (is_bool()) return std::get<bool>(v);
        if (is_int()) return std::get<std::int64_t>(v) != 0;
        if (is_float()) return std::get<double>(v) != 0.0;
        throw std::runtime_error("Value is not convertible to bool");
    }

    std::string to_string() const {
        std::ostringstream oss;
        if (is_int()) oss << std::get<std::int64_t>(v);
        else if (is_float()) oss << std::get<double>(v);
        else if (is_str()) oss << std::get<std::string>(v);
        else if (is_bool()) oss << (std::get<bool>(v) ? "true" : "false");
        else oss << "";
        return oss.str();
    }
};


// apply arithmetics to a and b with the operator op
Value apply_arith(const Value& a, const Value& b, char op) {
    if (op == '+' && (a.is_str() || b.is_str())) {
        std::string left = a.is_str() ? a.as_str() : a.to_string();
        std::string right = b.is_str() ? b.as_str() : b.to_string();
        return Value(left + right);
    }

    bool promotes_to_float = (a.is_float() || b.is_float() || op == '/');

    if (promotes_to_float) {
        double af = a.as_float();
        double bf = b.as_float();
        double res;
        switch (op) {
            case '+': res = af + bf; break;
            case '-': res = af - bf; break;
            case '*': res = af * bf; break;
            case '/':
                if (bf == 0.0) throw RuntimeError("Division by zero");
                res = af / bf; break;
            default: throw ParseError(std::string("Unknown binary operator: ") + op);
        }
        return Value(res);
    } else {
        std::int64_t ai = a.as_int();
        std::int64_t bi = b.as_int();
        switch (op) {
            case '+': return Value(ai + bi);
            case '-': return Value(ai - bi);
            case '*': return Value(ai * bi);
            case '/':
                if (bi == 0) throw RuntimeError("Division by zero");
                return Value(ai / bi);
            default: throw ParseError(std::string("Unknown binary operator: ") + op);
        }
    }
}





class runtime {
    std::vector<std::unique_ptr<ast::n>> nodesl;
    std::unordered_map<std::string, Value> vars;   // stores variables
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
    Value evaluateExpression(ast::n* node) {
        if (!node) throw ParseError("Null node in eval");

        if (node->type == "literal") {
            auto* litnode = dynamic_cast<ast::lit*>(node);
            if (!litnode) throw ParseError("Literal node cast failed");

            // check for literal value, if isnt valid return void
            if (std::holds_alternative<int>(litnode->literal)) {
                int v = std::get<int>(litnode->literal);
                return Value(static_cast<std::int64_t>(v));
            } else if (std::holds_alternative<std::string>(litnode->literal)) {
                return Value(std::get<std::string>(litnode->literal));
            } else if (std::holds_alternative<bool>(litnode->literal)) {
                return Value(std::get<bool>(litnode->literal));
            } else {
                return Value::Void();
            }
        }

        // handle binary operation
        if (node->type == "binaryop") {
            if (node->children.size() < 2) throw ParseError("Binary op missing operand");
            Value left = evaluateExpression(node->children[0].get());
            Value right = evaluateExpression(node->children[1].get());
            char op = (node->value.empty() ? '?' : node->value[0]);
            return apply_arith(left, right, op);
        }

        if (node->type == "ref") {
            auto it = vars.find(node->value);
            if (it == vars.end()) throw RuntimeError("Unknown variable: " + node->value);
            return it->second;
        }

        // if node is callable, check return value and return it
        if (node->type == "call") {
            std::vector<Value> arg_vals;
            for (const auto& child : node->children) arg_vals.push_back(evaluateExpression(child.get()));

            const std::string call_name = node->value;
            auto it_fn = functions.find(call_name);
            if (it_fn != functions.end()) {
                ast::n* fn_node = it_fn->second;
                auto params_ptr = fn_node->get_params();
                if (!params_ptr) throw RuntimeError("Function has no params container");
                auto& params = *params_ptr;
                if (params.size() != arg_vals.size()) throw RuntimeError("Argument count mismatch when calling function '" + call_name + "'");

                auto old_vars = vars;
                for (size_t i = 0; i < params.size(); ++i) {
                    const std::string& param_name = params[i].second;
                    vars.insert_or_assign(param_name, arg_vals[i]);
                }
                auto ret = run(fn_node->children);
                vars = std::move(old_vars);
                if (ret) return *ret;
                return Value::Void();
            }

            return Value::Void();
        }

        throw RuntimeError("Unsupported node type in expression: " + node->type);
    }



public:
    std::optional<Value> run(const std::vector<std::unique_ptr<ast::n>>& nodes) {
        if (nodes.empty()) {
            if (debug) std::cout << "[DEBUG] input empty\n";
            return std::nullopt;
        }

        for (const auto& node : nodes) {
            if (!node) continue;

            if (debug) std::cout << "[DEBUG] node.type: \"" << node->type << "\" node.value: \"" << node->value << "\"\n";

            if (node->type == "var") {
                std::string varname = node->value;
                if (!node->children.empty() && node->children[0]) {
                    Value assigned = evaluateExpression(node->children[0].get());
                    vars.insert_or_assign(varname, assigned);
                    if (debug) std::cout << "[DEBUG] assigned var '" << varname << "' = " << assigned.to_string() << "\n";
                } else {
                    throw RuntimeError("Variable \"" + varname + "\" declared without initializer");
                }
                continue;
            }

            if (node->type == "function") {
                if (debug) std::cout<<"[DEBUG] processing function \""<< node->value <<"\""<<"\n";
                functions.insert_or_assign(node->value, node.get());
                continue;
            }

            if (node->type == "return") {
                if (node->children.empty() || !node->children[0]) {
                    return std::make_optional(Value::Void());
                } else {
                    Value ret = evaluateExpression(node->children[0].get());
                    if (debug) std::cout << "[DEBUG] return value: " << ret.to_string() << "\n";
                    return std::make_optional(ret);
                }
            }

            if (node->type == "call") {
                // evaluate the call (value may be discarded if call used as statement)
                Value v = evaluateExpression(node.get());

                const std::string call_name = node->value;

                if (call_name == "IOds_print") {
                    // print each child expression's string representation
                   std::string buf="";
                    for (const auto& child : node->children) {
                        Value arg = evaluateExpression(child.get());
                        buf+=arg.to_string();
                    }
                    std::cout<<buf;
                    continue;
                }
                if (call_name == "IOds_println") {
                    std::string buf="";
                    for (const auto& child : node->children) {
                        Value arg = evaluateExpression(child.get());
                        buf+= arg.to_string();
                    }
                    std::cout <<buf<<"\n";
                    continue;
                }
                if (call_name == "IOes_print") {
                    std::string buf="";
                    for (const auto& child : node->children) {
                        Value arg = evaluateExpression(child.get());
                        buf+=arg.to_string();
                    }
                    std::cout<<buf;
                    continue;
                }
                if (call_name == "IOes_println") {
                    std::string buf="";
for (const auto& child : node->children) {
                        Value arg = evaluateExpression(child.get());
                        buf += arg.to_string();
                    }
                    std::cerr<<buf << "\n";
                    continue;
                }
                if (call_name == "tdebug") {
                    debug = !debug;
                    continue;
                }
                // otherwise ignore returned Value (already handled if inside expression)
                continue;
            }
            if (node->type == "for"){
                Value times = evaluateExpression(node->a().get());
                auto intv = times.as_int();
                auto &body = node->children;
                for(int i = 0; i<intv; i++){
                    run(body);
                }
                continue;
            }
        }

        return std::nullopt;
    }

}; // end runtime
