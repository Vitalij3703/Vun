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
    
};