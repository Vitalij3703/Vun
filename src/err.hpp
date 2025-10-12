#pragma once
#include <iostream>
#include <string>
using namespace std;
// i dont really plan on improving this but maybe with enough thinking ill do
class ParseError {
    public:
    ParseError(string text){
        cerr << "ParseError:\n\t"<<text<<endl;
    }
};
class RuntimeError {
    public:
    RuntimeError(string text){
        cerr << "RuntimeError:\n\t"<<text<<endl;
    }
};