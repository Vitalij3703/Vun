#include "runtime.hpp"
#include <iostream>
#include <string>
#include <cctype>

const std::string VER = "indev02";

void repl(){
    std::cout<<"Vun - REPL\nVersion: "<<VER<<"\nNote: The input must contain the full program youll want to run";
    while (true){
        std::string input;
        std::cout<<"$> ";std::cin >> input;
        auto r = runtime(input);
    }
}