#include <iostream>
#include "lexer.hpp"
using namespace std;
class ParseError {
    ParseError(tok token){
        cerr << "ParseError at token " << "{"<<token.type<<" : "<<token.value<<"}" <<endl;
    }
};