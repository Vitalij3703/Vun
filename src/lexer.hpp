// vun lexar

#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cctype>
using namespace std;
enum token_type {
    IDEF, LPAREN, RPAREN, LBRACE, RBRACE, KEYW,
    STR, INT, DOT, COMMA,
    EQUL, IS, DIV, MUL, MIN, PLU,
    SEMI, FE
};
struct tok {
    enum token_type type;
    string value;
};


class lexer {
    private:
            enum token_type identefier = IDEF;
            enum token_type keyword = KEYW;
            enum token_type left_paren = LPAREN;
            enum token_type right_paren = RPAREN;
            enum token_type left_brace = LBRACE;
            enum token_type right_brace = RBRACE;
            enum token_type _string = STR;
            enum token_type _int = INT;
            enum token_type dot = DOT;
            enum token_type equal = EQUL;
            enum token_type equals = IS;
            enum token_type divis = DIV;
            enum token_type multi = MUL;
            enum token_type plus = PLU;
            enum token_type minus = MIN;
            enum token_type semicolon = SEMI;
        //
        string inp;
        int pos = 0;
        char cchar;
    public:
        tok token(token_type type, string value){
            // this function is the result of a human x horse (lazy developer x deleted class).
            return {type, value};
        }
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
            return inp[++pos]; // returns next char
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
                cout << "making str, with "<<cchar<<endl;
                result+=cchar;
                adv();
            }
            cout << "result: " <<result<<endl;
            return result;
        }
        string id_build(){
            // builds an id
            string result;
            while (isalpha(cchar) || cchar == '_'){
                cout << "making id, with " << cchar<<endl;
                result+=cchar;
                adv();
            }
            cout << "result: "<<result;
            return result;
        }
        int num_build(){
            // builds an int
            vector<int> result;
            while (isdigit(cchar)){
                cout << "making int, with "<< cchar<<endl;
                result.push_back(cchar);
                adv();
            }
            string tstr;
            for (int digit : result){
                tstr+=to_string(digit);
            }
            cout << "result: " << tstr<<endl;
            return stoi(tstr);
        }
        vector<tok> tokenize(){
            // tokenizes input
            vector<tok> tokens;
            while (cchar != '\0'){
                cout << "processing char: "<<cchar<<endl;
                cout << "current pos: "<<to_string(get_pos());
                skipWS();
                if (isalpha(cchar)){
                    // if current char is a letter and not a string, its either a keyword or an id
                    string id = id_build();
                    if (id == "str" || id == "int" || id == "import" || id == "if" || id == "while" || id == "func" || id == "return"){
                        tokens.push_back(token(keyword, id));
                        
                    } else {
                        tokens.push_back(token(identefier, id));
                        
                    }
                    continue;
                }
                if(cchar == '('){
                    tokens.push_back(token(left_paren, "("));
                    adv();
                    continue;
                }
                if(cchar == ')'){
                    tokens.push_back(token(right_paren, ")"));
                    adv();
                    continue;
                }
                if(cchar == '{'){
                    tokens.push_back(token(left_paren, "{"));
                    adv();
                    continue;
                }
                if(cchar == '}'){
                    tokens.push_back(token(right_paren, "}"));
                    adv();
                    continue;
                }
                if(cchar == '"'){
                    string str = str_build();
                    tokens.push_back(token(_string, str));
                    adv();
                    continue;
                }
                if(isdigit(cchar)){
                    string numb = to_string(num_build());
                    string num = numb+"";
                    tokens.push_back(token(_int, num));
                    adv();
                    continue;
                }
                if(cchar == '.'){
                    tokens.push_back(token(dot, "."));
                    adv();
                    continue;
                }
                if(cchar == '='){
                    if (next() == '='){
                        tokens.push_back(token(equals, "=="));
                        adv();
                    } else {tokens.push_back(token(equal, "="));}
                    adv();
                }
                if(cchar == '/'){
                    tokens.push_back(token(divis, "/"));
                    adv();
                    continue;
                }
                if(cchar == '*'){
                    tokens.push_back(token(multi, "*"));
                    adv();
                    continue;
                }
                if(cchar == '-'){
                    tokens.push_back(token(minus, "-"));
                    adv();
                    continue;
                }
                if(cchar == '+'){
                    tokens.push_back(token(plus, "+"));
                    adv();
                    continue;
                }
                if (cchar == ',')
                {
                    tokens.push_back(token(token_type::COMMA, ","));
                    adv();
                }
                
                if(cchar == ';'){
                    tokens.push_back(token(semicolon, ";"));
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
                adv();
            }
            tokens.push_back(token(token_type::FE, "\0"));
            return tokens;
        }

};