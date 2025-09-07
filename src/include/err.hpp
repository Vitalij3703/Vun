#include <iostream>
#include "lexer.hpp"
using namespace std;

class ParseError {
    ParseError(int pos){
        cerr << "ParseError at pos " << pos <<endl;
    }
};
class RuntimeError {
    RuntimeError(int pos){
        cerr << "RuntimeError at pos " << pos <<endl;
    }
};