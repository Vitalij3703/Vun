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
    EQUL, IS, DIV, MUL, MIN, PLU, GRRT, LWR, LOE, GOE, NOT, OR, AND, LOUD, /* loud is !*/ NEQUL,
    SEMI, FE, _NULL
};
struct tok {
    enum token_type type;
    string value;
    tok(enum token_type t, string v):type(t),value(v){}
    tok():type(token_type::_NULL), value(""){}
    tok(enum token_type t):type(t), value(""){}
};


class lexer {
    private:
        string inp;
        int pos = 0;
        char cchar;
    public:
        lexer(string input){
            inp = input;
            cchar = inp[pos];
        }
        int get_pos(){return pos;}
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
            while (isalpha(cchar) || cchar == '_'){
                //cout << "making id, with " << cchar<<endl;
                result+=cchar;
                adv();
            }
            
            return result;
        }
        int num_build(){
            // builds a number
            string result;
            while (isdigit(cchar) || cchar == '.' || cchar == '_'){
                if(cchar == '_') adv();
                result+=cchar;
                adv();
            }
            return stof(result);
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
                    if (id == "str" || id == "int" || id == "null" || id == "if" || id == "for" || id == "func" || id == "return" || id == "void" || id == "bool" || id == "float"){
                        tokens.push_back({token_type::KEYW, id});
                    } else if(id == "true" || id == "false"){
                        tokens.push_back({token_type::BOOL, id});
                    } else {
                        tokens.push_back({token_type::IDEF, id});
                    }
                    continue;
                }
                if(cchar == '('){
                    tokens.push_back({token_type::LPAREN, "("});
                    adv();
                    continue;
                }
                if(cchar == ')'){
                    tokens.push_back({token_type::RPAREN, ")"});
                    adv();
                    continue;
                }
                if(cchar == '{'){
                    tokens.push_back({token_type::LBRACE, "{"});
                    adv();
                    continue;
                }
                if(cchar == '}'){
                    tokens.push_back({token_type::RBRACE, "}"});
                    adv();
                    continue;
                }
                if(cchar == '"'){
                    string str = str_build();
                    tokens.push_back({token_type::STR, str});
                    adv();
                    continue;
                }
                if(isdigit(cchar)){
                    float num = num_build();
                    if(floor(num) == num) tokens.push_back({token_type::INT, to_string(num)});
                    else {tokens.push_back({token_type::FLOAT, to_string(num)});}
                    continue;
                }
                if(cchar == '.'){
                    tokens.push_back({token_type::DOT});
                    adv();
                    continue;
                }
                if(cchar == '='){
                    if (next() == '='){
                        tokens.push_back({token_type::IS, "=="});
                        adv();
                    } else {tokens.push_back({token_type::EQUL, "="});}
                    adv();
                }
                if(cchar == '/'){
                    tokens.push_back({token_type::DIV, "/"});
                    adv();
                    continue;
                }
                if(cchar == '*'){
                    tokens.push_back({token_type::MUL, "*"});
                    adv();
                    continue;
                }
                if(cchar == '-'){
                    tokens.push_back({token_type::MIN, "-"});
                    adv();
                    continue;
                }
                if(cchar == '+'){
                    tokens.push_back({token_type::PLU, "+"});
                    adv();
                    continue;
                }
                if (cchar == ',')
                {
                    tokens.push_back({token_type::COMMA, ","});
                    adv();
                }
                
                if(cchar == ';'){
                    tokens.push_back({token_type::SEMI, ";"});
                    adv();
                    continue;
                }
                if (cchar == '/' && next() == '*'){
                    // comments
                    do {
                        adv();
                    } while(!(cchar == '*' && next() == '/' && next() != '\0'));
                    adv();
                    continue;
                }
                if (cchar == '<'){
                    if(next() == '='){
                        tokens.push_back({token_type::LOE, "<="});
                        adv();
                    }
                    else {
                        tokens.push_back({token_type::LWR, "<"});
                        adv();
                    }
                    continue;
                }
                if (cchar == '>'){
                    if(next() == '='){
                        tokens.push_back({token_type::GOE, ">="});
                        adv();
                    }
                    else {
                        tokens.push_back({token_type::GRRT, ">"});
                        adv();
                    }
                    continue;
                }
                if (cchar == '!'){
                    tokens.push_back({token_type::NOT, "!"});
                    adv();
                    continue;
                }
                if (cchar == '|'){
                    tokens.push_back({token_type::OR, "|"});adv();
                    continue;
                }
                if (cchar == '&'){
                    tokens.push_back({token_type::AND, "&"});adv();
                    continue;
                }
                if(cchar == '!'){if(!(next() == '=')){ tokens.push_back({token_type::LOUD, "!"}); adv();} else { tokens.push_back({token_type::NEQUL, "!="});adv();}; continue;}

                adv();
            }
            tokens.push_back({token_type::FE, "\0"});
            return tokens;
        }

};