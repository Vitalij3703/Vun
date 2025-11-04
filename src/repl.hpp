#include "runtime.hpp"
#include "parser.hpp"
#include <iostream>
#include <string>
#include <cctype>
#include <memory>
#include <vector>
#include "ast.hpp"

const std::string VER = "indev03";

void repl(){
    runtime rt("");
    std::cout<<"Vun - REPL\nVersion: "<<VER<<"\n";
        std::string input;
        std::cout<<"$> ";std::cin >> input;
        parser a(input);
        auto b = a.parse();
        rt.run(b);
        repl();
}