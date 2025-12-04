// vun lexar

#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <cmath>

using namespace std;
enum token_type {
    IDEF, LPAREN, RPAREN, LBRACE, RBRACE, KEYW,
    STR, INT, DOT, COMMA, FLOAT, BOOL,
    EQUL, IS, DIV, MUL, MIN, PLU, GRRT, LWR, LOE, GOE, NOT, OR, AND, LOUD, /* loud is !*/ NEQUL, MOD,
    SEMI, FE, _NULL
};
struct tok {
    enum token_type type;
    string value;
    unsigned long long pos;
    tok(enum token_type t, string v, unsigned long long p):type(t),value(v), pos(p){}
    tok():type(token_type::_NULL), value(""){}
    tok(enum token_type t):type(t), value(""){}
};


class lexer {
    private:
        string inp;
        long long unsigned int pos = 0;
        char cchar;
    public:
        lexer(string input){
            inp = input;
            cchar = inp[pos];
        }
        
        void adv(){
            if (pos < inp.size()){
                pos+=1;
                cchar = inp[pos];
            } else {cchar = '\0';}
        }
        char next(){
            return inp[pos+1]; // returns next char
        }
        void skipWS(){
            // skip whitespace
            if (isspace(cchar)){
                adv();
            }
        }

        string str_build(){
            // builds a string
            string result;
            adv();
            while (cchar != '"'){
                //cout << "making str, with "<<cchar<<endl;
                result+=cchar;
                adv();
            }
            //cout << "result: " <<result<<endl;
            return result;
        }
        string id_build(){
            // builds an id
            string result;
            while (isalpha(cchar) || cchar == '_' || isdigit(cchar)){
                //cout << "making id, with " << cchar<<endl;
                result+=cchar;
                adv();
            }
            
            return result;
        }
        double num_build(){
            // builds a number
            string result;
            while (isdigit(cchar) || cchar == '.' || cchar == '_'){
                if(cchar == '_') adv();
                result+=cchar;
                adv();
            }
            return stod(result);
        }
        vector<tok> tokenize(){
            // tokenizes input
            vector<tok> tokens;
            while (cchar != '\0'){
                /*cout << "processing char: "<<cchar<<endl;
                cout << "current pos: "<<to_string(get_pos()) << endl;*/
                skipWS();
                if (isalpha(cchar)){
                    // if current char is a letter and not a string, its either a keyword or an id
                    string id = id_build();
                    if (id == "str" || id == "int" || id == "nul" || id == "if" || id == "for" || id == "func" || id == "return" || id == "void" || id == "bool" || id == "float" || id == "while" || id == "var" || id == "const"){
                        tokens.push_back({token_type::KEYW, id, pos});
                    } else if(id == "true" || id == "false"){
                        tokens.push_back({token_type::BOOL, id, pos});
                    } else {
                        tokens.push_back({token_type::IDEF, id, pos});
                    }
                    continue;
                }
                if(cchar == '('){
                    tokens.push_back({token_type::LPAREN, "(", pos});
                    adv();
                    continue;
                }
                if(cchar == ')'){
                    tokens.push_back({token_type::RPAREN, ")", pos});
                    adv();
                    continue;
                }
                if(cchar == '{'){
                    tokens.push_back({token_type::LBRACE, "{", pos});
                    adv();
                    continue;
                }
                if(cchar == '}'){
                    tokens.push_back({token_type::RBRACE, "}", pos});
                    adv();
                    continue;
                }
                if(cchar == '"'){
                    string str = str_build();
                    tokens.push_back({token_type::STR, str, pos});
                    adv();
                    continue;
                }
                if(isdigit(cchar)){
                    double num = num_build();
                    if(floor(num) == num) tokens.push_back({token_type::INT, to_string(static_cast<int>(num)), pos});
                    else {tokens.push_back({token_type::FLOAT, to_string(num), pos});}
                    continue;
                }
                if(cchar == '.'){
                    tokens.push_back({token_type::DOT, ".", pos});
                    adv();
                    continue;
                }
                if(cchar == '='){
                    if (next() == '='){
                        tokens.push_back({token_type::IS, "==", pos});
                        adv();
                    } else {tokens.push_back({token_type::EQUL, "=", pos});}
                    adv();
                }
                if(cchar == '/'){
                    tokens.push_back({token_type::DIV, "/", pos});
                    adv();
                    continue;
                }
                if(cchar == '*'){
                    tokens.push_back({token_type::MUL, "*", pos});
                    adv();
                    continue;
                }
                if(cchar == '#'){
                    // comments
                    do {adv();}
                    while(cchar!='#');
		    adv();
                    continue;
                }
                if(cchar == '-'){
                    tokens.push_back({token_type::MIN, "-", pos});
                    adv();
                    continue;
                }
                if(cchar == '+'){
                    tokens.push_back({token_type::PLU, "+", pos});
                    adv();
                    continue;
                }
                if (cchar == ',')
                {
                    tokens.push_back({token_type::COMMA, ",", pos});
                    adv();
                }
                
                if(cchar == ';'){
                    tokens.push_back({token_type::SEMI, ";", pos});
                    adv();
                    continue;
                }
                if (cchar == '<'){
                    if(next() == '='){
                        tokens.push_back({token_type::LOE, "<=", pos});
                        adv();
                    }
                    else {
                        tokens.push_back({token_type::LWR, "<", pos});
                        adv();
                    }
                    continue;
                }
                if (cchar == '>'){
                    if(next() == '='){
                        tokens.push_back({token_type::GOE, ">=", pos});
                        adv();
                    }
                    else {
                        tokens.push_back({token_type::GRRT, ">", pos});
                        adv();
                    }
                    continue;
                }
                if (cchar == '!'){
                    tokens.push_back({token_type::NOT, "!", pos});
                    adv();
                    continue;
                }
                if (cchar == '|'){
                    tokens.push_back({token_type::OR, "|", pos});adv();
                    continue;
                }
                if (cchar == '&'){
                    tokens.push_back({token_type::AND, "&", pos});adv();
                    continue;
                }
                if(cchar == '!'){if(!(next() == '=')){ tokens.push_back({token_type::LOUD, "!", pos}); adv();} else { tokens.push_back({token_type::NEQUL, "!=", pos});adv();}; continue;}
                if(cchar == '%'){tokens.push_back({token_type::MOD, "%", pos});continue;}
                adv();
            }
            tokens.push_back({token_type::FE, "\0", pos});
            return tokens;
        }

};
