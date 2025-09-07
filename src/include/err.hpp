#include <iostream>
#include "lexer.hpp"
using namespace std;
struct posit{
    int line; int col;
};
class ParseError {
    ParseError(posit pos){
        cerr << "ParseError at pos " << "{"<<"line "<<pos.line<<" collum "<<pos.col<<"}" <<endl;
    }
};