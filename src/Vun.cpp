
#include "include/runtime.hpp"
#include <iostream>
#include <fstream>
#include <string>
int main(int argc, char **argv){
    if (argc < 2){
    	std::cerr << "Usage: vun <file.vun>";
    }
    std::fstream file(argv[2]);
    std::string buffer;
    while (std::getline(file, buffer)){
        //idk do something
    }
    file.close();
    runtime run = runtime(buffer);
    run.run();
    return 0;
}