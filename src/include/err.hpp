#pragma once
#include <iostream>
#include "lexer.hpp"
using namespace std;
// i dont really plan on improving this but maybe with enough thinking ill do
class ParseError {
    public:
    ParseError(tok token){
        cerr << "ParseError at token: "<<rtv(token)<<endl;
    }
};
class RuntimeError {
    public:
    RuntimeError(){
        cerr << "RuntimeError. please find what caused it."<<endl;
    }
};