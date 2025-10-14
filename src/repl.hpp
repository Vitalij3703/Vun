#include "runtime.hpp"
#include <iostream>
#include <string>
#include <cctype>

const std::string VER = "indev";

void repl(){
    std::cout<<"Vun - REPL\nVersion: "<<VER<<"\n";
    while (true){
        std::string input;
        std::cout<<">$ ";std::cin >> input;
        auto r = runtime(input);
    }
}