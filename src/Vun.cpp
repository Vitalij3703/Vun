
#include "runtime.hpp"
#include "repl.hpp"
#include <iostream>
#include <string>
#include "utils.hpp"
#define str char*
int main(int argc, char *argv[]){
    if (argc < 2){
        repl();
    }
    bool debug = false;
    str argv1 = argv[1];
    str argv2 = argv[2];
    if(argv2=="-d") debug = true;
    if (argv1 == "--help" || argv1 == "-h" || argv1 == "-?"){
	    std::cout<<"Usage: vun <file.vun>\nOther stuff:\n\tvun --help/-h/-?: display this message\n\tvun <file.vun> -d: turn on debug"<<std::endl;}
    runtime run(gfc(argv[1], debug), argv[1]);
    return 0;
}
