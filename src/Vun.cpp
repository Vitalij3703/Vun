
#include "runtime.hpp"
#include "repl.hpp"
#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char *argv[]){
    if (argc < 2){
    	std::cout << "Usage: vun <file.vun>\n";
        repl();
    }
    std::ifstream file(argv[1]);
    std::string buffer = "";
    if (file.is_open()){
        std::string line;
        while (std::getline(file, line)){
            buffer += line;
        }
    } else {
        std::cout <<"File doesnt exist.\n";
        return 1;
    }
    runtime run(buffer);
    file.close();
    return 0;
}