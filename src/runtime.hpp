#include "parser.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include "ast.hpp"
#include "err.hpp"
#include <unordered_map>

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
    //                 name         value                ||
    std::unordered_map<std::string, ast::n*> functions; // stores functions so other nodes can access it

public:
    runtime(const std::string& in) {
        parser p(in);
        if (debug) std::cout << "[DEBUG] input size: " <<in.size()<<"\n";
        nodesl = p.parse();
        if (debug) std::cout << "[DEBUG] parsed nodes count: " << nodesl.size() << "\n";
        run(std::move(nodesl));
    }

void run(std::vector<std::unique_ptr<ast::n>> nodes) {
    if (nodes.empty()) {
        if (debug) std::cout << "[DEBUG] input empty\n";
        return;
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
                std::string assigned = node->children[0]->value;
                vars.insert_or_assign(varname, assigned);
                if (debug) std::cout << "[DEBUG] assigned var '" << varname << "' with the value \"" << assigned << "\"\n";
            } else {
                throw RuntimeError("Variable \""+varname+"\" declared without initizer");
            }
        }
        if (node->type == "literal") {
            if (debug) std::cout << "[DEBUG] literal value: \"" << node->value << "\"\n";
        }
        if (node->type == "function"){
            if (debug) std::cout << "[DEBUG] processing function: \""<<node->value<<"\"\n";
            functions.insert_or_assign(node->value, node.get()); // inserts function into functions so other nodes can access it

        }
        // finally the fun part
        // the builtins
        if (node->type == "call"){
            std::string call_name = node->value;
            std::vector<std::unique_ptr<ast::n>> args = std::move(node->children);

            auto it_fn = functions.find(call_name);
            if (it_fn != functions.end()) {
                ast::n* fn_node = it_fn->second;
                auto params_ptr = fn_node->get_params();
                if (!params_ptr) throw RuntimeError("Function has no params container");
                auto& params = *params_ptr;
            
                if (params.size() != args.size()) {
                    throw RuntimeError("Argument count mismatch when calling function '" + call_name + "'");
                }
                auto old_vars = vars;
                for (size_t i = 0; i < params.size(); ++i) {
                    const std::string& param_name = params[i].second;
                    const std::string& expected_type = params[i].first;
                
                    ast::n* arg = args[i].get();
                    std::string value_to_store;
                
                    if (!arg) throw RuntimeError("Null argument passed");
                
                    if (arg->type == "literal") {
                        std::string actual_type;
                        if (arg->is_str_lit()) actual_type = "str";
                        else if (arg->is_int_lit()) actual_type = "int";
                        else if (arg->is_bool_lit()) actual_type = "bool";
                        else actual_type = "void";
                    
                        if (expected_type != actual_type) {
                            throw RuntimeError("Expected argument of type '" + expected_type +
                                "' but got '" + actual_type + "' for param '" + param_name + "'");
                        }
                        value_to_store = arg->value;
                    } else if (arg->type == "binaryop") {
                        float f = pbexpr(arg);
                        if (expected_type == "int") {
                            value_to_store = std::to_string(static_cast<long long>(f));
                        } else {
                            value_to_store = std::to_string(f);
                        }
                    } else if (arg->type == "ref") {
                        auto vit = vars.find(arg->value);
                        if (vit == vars.end()) throw RuntimeError("Unknown variable: " + arg->value);
                        value_to_store = vit->second;
                    } else {
                        throw RuntimeError("Unsupported arg type for function call: " + arg->type);
                    }
                    vars.insert_or_assign(param_name, value_to_store);
                }
                run(std::move(fn_node->children));
                vars = std::move(old_vars);
            }    
            // prints the args of the callable to default stream
            if (call_name == "IOds_print"){
                for(auto& arg : args){
                    if(arg->type == "literal"){
                        std::cout << arg->value;
                    }
                    else if (arg->type == "binaryop"){
                        std::cout << std::to_string(pbexpr(arg.get()));
                    }
                    else if (arg->type == "ref") {
                        for (auto& var : vars){
                            if (arg->value == var.first){
                                std::cout << var.second;
                            }
                        }
                    }
                    
                }
            }
            // same as IOds_print but also adds a newline whitespace at the end
            if (call_name == "IOds_println"){
                for(auto& arg : args){
                    if(arg->type == "literal"){
                        std::cout << arg->value << "\n";
                    }
                    else if (arg->type == "binaryop"){
                        std::cout << std::to_string(pbexpr(arg.get())) << "\n";
                    }
                    else if (arg->type == "ref") {
                        for (auto& var : vars){
                            if (arg->value == var.first){
                                std::cout << var.second << "\n";
                            }
                        }
                    }
                }
            }


            // prints the args to error stream
            if (call_name == "IOes_print"){
                for(auto& arg : args){
                    if(arg->type == "literal"){
                        std::cerr << arg->value;
                    }
                    else if (arg->type == "binaryop"){
                        std::cerr << std::to_string(pbexpr(arg.get()));
                    }
                    else if (arg->type == "ref") {
                        for (auto& var : vars){
                            if (arg->value == var.first){
                                std::cerr << var.second;
                            }
                        }
                    }
                }
            }
            // same as IOes_print but also adds a newline whitespace at the end
            if (call_name == "IOes_println"){
                for(auto& arg : args){
                    if(arg->type == "literal"){
                        std::cerr << arg->value << "\n";
                    }
                    if (arg->type == "binaryop"){
                        std::cerr << std::to_string(pbexpr(arg.get())) << "\n";
                    }
                    else if (arg->type == "ref") {
                        for (auto& var : vars){
                            if (arg->value == var.first){
                                std::cerr << var.second << "\n";
                            }
                        }
                    }
                }
            }
            // toggles debug
            if (call_name == "tdebug") debug = !debug;
        }
    }
}
};
