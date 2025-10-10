#include "parser.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include "ast.hpp"

class runtime {
    std::vector<std::unique_ptr<ast::n>> nodes;

public:
    runtime(const std::string& in) {
        parser p(in);
        std::cout << "[DEBUG] input size: " <<in.size()<<"\n";
        nodes = p.parse();
        std::cout << "[DEBUG] parsed nodes count: " << nodes.size() << "\n";
    }

void run() {
    if (nodes.empty()) {
        std::cout << "[DEBUG] input empty\n";
        return;
    }
    for (const auto& node : nodes) {
        if (!node) {
            std::cout << "[DEBUG] null node\n";
            continue;
        }
        std::cout << "[DEBUG] node.type: \"" << node->type << "\" node.value: \"" << node->value << "\"\n";
        if (node->type == "var") {
            std::string varname = node->value;
            if (!node->children.empty() && node->children[0]) {
                std::string assigned = node->children[0]->value;
                std::cout << "[DEBUG] assigned var '" << varname << "' with the value \"" << assigned << "\"\n";
            } else {
                std::cout << "[DEBUG] variable '" << varname << "' declared without initializer\n";
            }
        }
        if (node->type == "literal") {
            std::cout << "[DEBUG] literal value = \"" << node->value << "\"\n";
        }
        // finally the fun part
        // the builtins
        if (node->type == "call"){
            std::string call_name = node->value;
            std::vector<std::unique_ptr<ast::n>> args = std::move(node->children);
            
            
            // prints the (literal)args of the callable to default stream
            if (call_name == "IOds_print"){
                for(auto& arg : args){
                    if(arg->type == "literal"){
                        std::cout << arg->value;
                    }
                }
            }
            // same as IOds_print but also adds a newline whitespace at the end
            if (call_name == "IOds_println"){
                for(auto& arg : args){
                    if(arg->type == "literal"){
                        std::cout << arg->value << "\n";
                    }
                }
            }


            // prints the (literal)args to error stream
            if (call_name == "IOes_print"){
                for(auto& arg : args){
                    if(arg->type == "literal"){
                        std::cerr << arg->value;
                    }
                }
            }
            // same as IOes_print but also adds a newline whitespace at the end
            if (call_name == "IOes_println"){
                for(auto& arg : args){
                    if(arg->type == "literal"){
                        std::cerr << arg->value << "\n";
                    }
                }
            }

            
        }
    }
}
};
