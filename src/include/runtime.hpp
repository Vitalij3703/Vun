#include "parser.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include "ast.hpp"

class runtime {

std::vector<std::unique_ptr<ast::n>> nodes;

public:
    runtime(std::string in){
        parser p(in);
        nodes = p.parse();
    }
void run(){
    for(const auto& node : nodes){
    	if (node->type == "var"){
    		std::cout << "[DEBUG] assigned var with the value "<<node->value<<"\n";
    	}
    }
}};