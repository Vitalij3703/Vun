// main vun file
#include "include/runtime.hpp"
#include <iostream>
#include <fstream>
#include <string>
int main(int argc, char *argv[]){
    std::fstream file(argv[1]);
    int i{0};
    std::string buffer;
    while (!(fstream::cur == '\0')){
        file.seekg(i);
        i++;buffer += fstream::cur;
    }
    runtime run = runtime(buffer);
    return 0;
}