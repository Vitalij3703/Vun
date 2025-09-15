#pragma once
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
            lexer l=lexer(in);
            this->input = l.tokenize();
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
            if (ct.type == excepted_token && ct.value == excepted_char){
                return true;
            }
            return false;
        }
        bool match(token_type excepted_token){
            if (ct.type == excepted_token){
                return true;
            }
            return false;
        }
        bool consume(token_type excepted_token, string exceptedchar){
            if (ct.type == excepted_token && ct.value == exceptedchar){
                return true;
            } else {
                new ParseError(ct);
            }
            return false;
        }
        bool consume(token_type excepted_token){
            if (ct.type == excepted_token){
                return true;
            } else {
                new ParseError(ct);
            }
            return false;
        }
        // YES im vitalij, YES im making this off pseudo code
        // this probably will have logic issues
        ast::n parse_stat(){
            if (match(token_type::KEYW, "int") || match(token_type::KEYW, "str")){
                adv();
                if (match(token_type::IDEF)){
                    string name = ct.value;
                    adv();
                    if (match(token_type::EQUL)){
                        ast::n expr = parse_expr();
                        return ast::var(name, expr);
                    }
                }
            }
            return parse_expr();
        }
        ast::n parse_expr(){
            ast::n node = parse_term();
            while ((match(token_type::PLU)) || (match(token_type::MIN)))
            {
                tok op = ct;
                match(op.type);
                ast::n right = parse_term();
                node = ast::ben(node, op.value[0], right);
            }
            return node;
            
        }

        ast::n parse_term(){
            ast::n node = parse_fact();
            while (match(token_type::MUL) || match(token_type::DIV))
            {
                tok op = ct;
                match(op.type);
                ast::n right = parse_fact();
                node = ast::ben(node, op.value[0], right);
            }
            return node;
        }
        ast::n parse_fact(){
            if (ct.type == token_type::INT){
                int value = stoi(ct.value);
                match(token_type::INT);
                return ast::lit(value);
            }
            else if (ct.type == token_type::LPAREN){
                match(token_type::LPAREN);
                ast::n node = parse_expr();
                match(token_type::RPAREN);
                return node;
            }
            else {
                new ParseError(ct);
            }
        }


};


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

// func int art() is function that returns a data type of int at runtime with bla bla bla
*/