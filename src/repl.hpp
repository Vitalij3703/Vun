#include "runtime.hpp"
#include "parser.hpp"
#include <iostream>
#include <string>
#include <cctype>
#include <memory>
#include <vector>
#include "ast.hpp"

const std::string VER = "1.1";

void repl(){
    runtime rt = runtime("", "none");
    std::cout<<"Vun - (c) 2025 Vitalij Shapoval, licensed under the MIT license\nVersion: "<<VER<<"\n";
    while(true){
        std::string input;
        std::cout<<"> ";std::cin >> input;
        parser a(input, false);
        std::vector<std::unique_ptr<ast::n>> b;
        try{b = a.parse();} catch (...){std::cout<<"nul/error\n";}
        try{rt.run(b, false);} catch(...){std::cout<<"nul/error\n";}
        
    }
}