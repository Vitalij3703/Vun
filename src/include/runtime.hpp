#include "parser.hpp"
#include <string>
#include <iostream>
#include <optional>

class runtime {
    std::string input;
public:
    runtime(std::string in){
        this->input=in;
        parser p = parser(in);
    }
    
};