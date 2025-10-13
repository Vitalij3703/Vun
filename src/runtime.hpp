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
    if (!(expr->type=="binaryop")) return 0.0f;
    auto* left = expr->children[0].get();
    int leftval;
    auto* right = expr->children[1].get();
    int rightval;
    std::string op = expr->value;
    float result;
    if (op == OPS[0]){
        if (left->type == "literal"){
            if (left->is_int_lit()){
                leftval = std::stoi(left->value);
            }
        }
        else {
            leftval = static_cast<int>(pbexpr(left));
        }
        if (right->type == "literal"){
            if (right->is_int_lit()){
                rightval = std::stoi(right->value);
            }
        }
        else {
            rightval = static_cast<int>(pbexpr(right));
        }
        result = leftval + rightval;
        return result;
    }
    if (op == OPS[1]){
        if (left->type == "literal"){
            if (left->is_int_lit()){
                leftval = std::stoi(left->value);
            }
        }
        else {
            leftval = static_cast<int>(pbexpr(left));
        }
        if (right->type == "literal"){
            if (right->is_int_lit()){
                rightval = std::stoi(right->value);
            }
        }
        else {
            rightval = static_cast<int>(pbexpr(right));
        }
        result = leftval - rightval;
        return result;
    }
    if (op == OPS[2]){
        if (left->type == "literal"){
            if (left->is_int_lit()){
                leftval = std::stoi(left->value);
            }
        }
        else {
            leftval = static_cast<int>(pbexpr(left));
        }
        if (right->type == "literal"){
            if (right->is_int_lit()){
                rightval = std::stoi(right->value);
            }
        }
        else {
            rightval = static_cast<int>(pbexpr(right));
        }
        result = leftval * rightval;
        return result;
    }
    if (op == OPS[3]){
        if (left->type == "literal"){
            if (left->is_int_lit()){
                leftval = std::stoi(left->value);
            }
        }
        else {
            leftval = static_cast<int>(pbexpr(left));
        }
        if (right->type == "literal"){
            if (right->is_int_lit()){
                rightval = std::stoi(right->value);
            }
        }
        else {
            rightval = static_cast<int>(pbexpr(right));
        }
        result = leftval / rightval;
        return result;
    }
    throw ParseError("Invalid binary expression");

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

            for (auto& fn : functions){
                if (fn.first == call_name){
                    run(std::move(fn.second->children));
                }
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
