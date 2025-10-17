#pragma once
#include <iostream>
#include <string>
using namespace std;
// i dont really plan on improving this but maybe with enough thinking ill do
class ParseError {
    public:
    ParseError(string text){
        cerr << "\033[1;31mParseError:\n\t"<<text<<"\033[0m\n"<<endl;
    }
};
class RuntimeError {
    public:
    RuntimeError(string text){
        cerr << "\033[1;31mRuntimeError:\n\t"<<text<<"\033[0m\n"<<endl;
    }
};