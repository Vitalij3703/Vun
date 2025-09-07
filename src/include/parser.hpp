#include <iostream>
#include <string>
#include "lexer.hpp"
#include <vector>
#include "err.hpp"
using namespace std;
// the fucking parser
class parser {
    private:
        vector<tok> input;
        size_t ipos=0;
        tok ct;
    public:
        parser(string in){
            lexer l=lexer();
            this.input = l.tokenize();
            ct = input[ipos];
        }
        void adv(){
            ipos++;
            ct = input.at(ipos);
        }
        tok next(){
            return input[++ipos];
        }
        bool match(token_type excepted_token, string excepted_char){
            if (tok.type == excepted_token && tok.value == excepted_char){
                return true;
            }
            return false;
        }
        bool match(token_type excepted_token){
            if (tok.type == excepted_token){
                return true;
            }
            return false;
        }
        bool consume(token_type excepted_token, string exceptedchar){
            if (tok.type == excepted_token && tok.value == exceptedchar){
                return true;
            } else {
                new ParseError(ipos);
            }
            return false
        }
        bool consume(token_type excepted_token){
            if (tok.type == excepted_token){
                return true;
            } else {
                new ParseError(ipos);
            }
            return false
        }

}


/*
syntax that will be:





loadlib "siol";
loadlib "os";
func int art(int os.argc, str os.argv[]){
    output("Hello, World!");
    return 0;
}
// comment
//* multi line 
    comment \\*

// func int art() is function that returns a data type of int at runtime with 
*/