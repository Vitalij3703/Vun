
#include "include/runtime.hpp"
#include <iostream>
#include <fstream>
#include <string>
int main(int argc, char **argv[]){
    std::fstream file(argv[1]);
    std::string buffer;
    while (std::getline(file, buffer)){
        //idk do something
    }
    file.close();
    runtime run = runtime(buffer);
    return 0;
}