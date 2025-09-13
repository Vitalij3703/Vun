// vun lexar
// yes im too lazy to remove the "temp" parts
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
// debug (temp)
string tt_to_string(token_type type){
    switch (type)
    {
    case token_type::IDEF:
        return "IDEF";
        break;
    case token_type::LPAREN:
        return "LPRAREN";
        break;
    case token_type::RPAREN:
        return "RPAREN";
        break;
    case token_type::LBRACE:
        return "LBRACE";
        break;
    case token_type::RBRACE:
        return "RBRACE";
        break;
    case token_type::KEYW:
        return "KEYW";
        break;
    case token_type::STR:
        return "STR";
        break;
    case token_type::INT:
        return "INT";
        break;
    case token_type::DOT:
        return "DOT";
        break;
    case token_type::EQUL:
        return "EQUL";
        break;
    case token_type::IS:
        return "IS";
        break;
    case token_type::DIV:
        return "DIV";
        break;
    case token_type::MUL:
        return "MUL";
        break;
    case token_type::MIN:
        return "MIN";
        break;
    case token_type::PLU:
        return "PLU";
        break;
    case token_type::SEMI:
        return "SEMI";
        break;
    }
}
string rtv(tok toke){
    /* helo wolrd */
    return string("{")+tt_to_string(toke.type) +" | "+toke.value+"}";
} // end debug

class lexer {
    private:
        // token types (temp):
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
            cchar = inp.at(pos);
        }
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
            cout << "result: " <<result;
            return result;
        }
        string id_build(){
            // builds an id
            string result;
            while (isalpha(cchar)){
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
                tstr+=digit;
            }
            cout << "result: " << tstr;
            return stoi(tstr);
        }
        vector<tok> tokenize(){
            // tokenizes input
            vector<tok> tokens;
            while (cchar != '\0'){
                cout << "processing char: "<<cchar<<endl;
                skipWS();
                if (isalpha(cchar)){
                    // if current char is a letter and not a string, its either a keyword or an id
                    string id = id_build();
                    if (id == "str" || id == "int" || id == "_k" || id == "if" || id == "while" || id == "func" || id == "return"){
                        tokens.push_back(token(keyword, id));
                        
                    } else {
                        tokens.push_back(token(identefier, id));
                        
                    }
                }
                else if(cchar == '('){
                    tokens.push_back(token(left_paren, "("));
                    
                }
                else if(cchar == ')'){
                    tokens.push_back(token(right_paren, ")"));
                    
                }
                else if(cchar == '{'){
                    tokens.push_back(token(left_paren, "{"));
                    
                }
                else if(cchar == '}'){
                    tokens.push_back(token(right_paren, "}"));
                    
                }
                else if(cchar == '"'){
                    string str = str_build();
                    tokens.push_back(token(_string, str));
                    
                }
                else if(isdigit(cchar)){
                    int numb = num_build();
                    string num = numb +"";
                    tokens.push_back(token(_int, num));
                    
                }
                else if(cchar == '.'){
                    tokens.push_back(token(dot, "."));
                    
                }
                else if(cchar == '='){
                    if (next() == '='){
                        tokens.push_back(token(equals, "=="));
                    } else {tokens.push_back(token(equal, "="));}
                }
                else if(cchar == '/'){
                    tokens.push_back(token(divis, "/"));
                    
                }
                else if(cchar == '*'){
                    tokens.push_back(token(multi, "*"));
                    
                }
                else if(cchar == '-'){
                    tokens.push_back(token(minus, "-"));
                    
                }
                else if(cchar == '+'){
                    tokens.push_back(token(plus, "+"));
                    
                }
                else if (cchar == ',')
                {
                    tokens.push_back(token(token_type::COMMA, ","));
                }
                
                else if(cchar == ';'){
                    tokens.push_back(token(semicolon, ";"));
                    
                }
                adv();
            }
            tokens.push_back(token(token_type::FE, "\0"));
            return tokens;
        }

};