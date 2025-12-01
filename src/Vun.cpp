
#include "runtime.hpp"
#include "repl.hpp"
#include <iostream>
#include <string>
#include "utils.hpp"
int main(int argc, char *argv[]){
    if (argc < 2){
        repl();
    }
    std::string argv1 = argv[1];
    if (argv1 == "--help" || argv1 == "-h" || argv1 == "-?"){
	    std::cout<<"Usage: vun <file.vun>\nOther stuff:\n\tvun < --help/-h/-? : display this message"<<std::endl;}
    runtime run(gfc(argv[1], false), argv[1]);
    return 0;
}
