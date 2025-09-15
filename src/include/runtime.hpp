#include "parser.hpp"
#include <string>
#include <iostream>

class runtime {
    std::string input;
public:
    runtime(std::string in){
        this->input=in;
        parser p = parser(in);
    }
    
};