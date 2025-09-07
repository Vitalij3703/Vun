#include <iostream>
#include "lexer.hpp"
using namespace std;

class ParseError {
    ParseError(posit pos){
        cerr << "ParseError at pos " << "{"<<"line "<<pos.line<<" collum "<<pos.col<<"}" <<endl;
    }
};