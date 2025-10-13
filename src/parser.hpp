
#pragma once
#include <iostream>
#include <string>
#include "lexer.hpp"
#include <vector>
#include "err.hpp"
#include <memory>
#include "ast.hpp"
#include <utility>
using namespace std;

class parser {
private:
    vector<tok> input;
    size_t ipos = 0;
    tok ct;

public:
    
    parser(string in) {
        lexer l = lexer(in);
        this->input = l.tokenize();
        
        if (input.empty()) throw ParseError("Either, input empty or the lexer's at failure."); 
        ct = input[ipos];
    }

    void adv() {
        ipos++;
        if (ipos < input.size()) ct = input.at(ipos);
        else ct = input.back(); 
    }

    tok next() {
        
        adv();
        return ct;
    }

    bool match(token_type expected_token, string expected_char) {
        return ct.type == expected_token && ct.value == expected_char;
    }

    bool match(token_type expected_token) {
        return ct.type == expected_token;
    }

    bool match_next(token_type expected_token, const string& expected_char) {
        if (ipos + 1 >= input.size()) return false;
        const tok &t = input[ipos + 1];
        return t.type == expected_token && t.value == expected_char;
    }

    bool match_next(token_type expected_token) {
        if (ipos + 1 >= input.size()) return false;
        return input[ipos + 1].type == expected_token;
    }

    bool consume(token_type expected_token, string expected_char) {
        if (match(expected_token, expected_char)) {adv();return true;};
        throw ParseError("Consume expectation wasnt met. Position: "+to_string(ipos));
    }

    bool consume(token_type expected_token) {
        if (match(expected_token)) {adv();return true;};
        throw ParseError("Consume exceptation wasnt met. Position: "+to_string(ipos));
    }


    vector<unique_ptr<ast::n>> parse() {
        vector<unique_ptr<ast::n>> nlist;
        while (ct.type != token_type::FE){
            unique_ptr<ast::n> node = parse_stat();
            if (node){
                nlist.push_back(std::move(node));
            } else {
                
                throw ParseError("Nothing to parse.");
            }
        }
        return nlist;
    }

    
    unique_ptr<ast::n> parse_stat() {
        if (match(token_type::KEYW, "int") || match(token_type::KEYW, "str")) {
            auto t = ct.value;
            adv(); 
            if (match(token_type::IDEF)) {
                string name = ct.value;
                adv(); 
                if (match(token_type::EQUL)) {
                    adv(); 
                    auto expr = parse_expr();
                    consume(token_type::SEMI);
                    return make_unique<ast::var>(name, std::move(expr), t);
                }
                return make_unique<ast::var>(name, unique_ptr<ast::n>(nullptr), t);
            }
        }
        else if (match(token_type::IDEF) && match_next(token_type::LPAREN)){
            return parse_call();
        }
        else if (match(token_type::KEYW, "func")){
            return parse_func();
        }
        else if (match(token_type::KEYW, "return")) return parse_fuck();
        return parse_expr();
    }

    unique_ptr<ast::n> parse_func(){
        consume(token_type::KEYW, "func");
        if (!(match(token_type::KEYW, "str") || match(token_type::KEYW, "int") || match(token_type::KEYW, "void")))
            throw ParseError("Expected return type.");
        string ret_type = ct.value;
        adv();
        if (!match(token_type::IDEF)) throw ParseError("Expected function name");
        string name = ct.value;
        adv();
        consume(token_type::LPAREN);

        vector<pair<string,string>> params;

        if (!match(token_type::RPAREN)) {
            do {
                string type_buf, name_buf;
                if (!(match(token_type::KEYW, "int") || match(token_type::KEYW, "str")))
                    throw ParseError("Expected parameter type.");
                type_buf = ct.value;
                adv();
                if (!match(token_type::IDEF)) throw ParseError("Expected parameter name");
                name_buf = ct.value;
                adv();
                params.emplace_back(type_buf, name_buf);
                if (match(token_type::COMMA)) adv();
                else break;
            } while (true);
        }
        consume(token_type::RPAREN);
        consume(token_type::LBRACE);
        vector<unique_ptr<ast::n>> body;
        while (!match(token_type::RBRACE)) {
            body.push_back(parse_stat());
        }
        consume(token_type::RBRACE);
        return make_unique<ast::fn>(name, std::move(params), std::move(body));
    }
    unique_ptr<ast::n> parse_fuck() {
        consume(token_type::KEYW, "return");
        auto expr = parse_expr();
        consume(token_type::SEMI);
        return make_unique<ast::n>("return", make_vector(std::move(expr)));
    }

    static vector<unique_ptr<ast::n>> make_vector(unique_ptr<ast::n> node) {
        vector<unique_ptr<ast::n>> v;
        v.push_back(std::move(node));
        return v;
    }
    unique_ptr<ast::n> parse_call() {
        
        string func_name = ct.value;
        adv(); 
        consume(token_type::LPAREN);
        vector<unique_ptr<ast::n>> args;
        if (!match(token_type::RPAREN)) {
            do {
                args.push_back(std::move(parse_expr()));
                if (match(token_type::COMMA)) adv();
                else break;
            } while (true);
        }
        consume(token_type::RPAREN);
        consume(token_type::SEMI);
        return make_unique<ast::call>(func_name, std::move(args));
    }

    
    unique_ptr<ast::n> parse_expr() {
        auto node = parse_term();
        while (match(token_type::PLU) || match(token_type::MIN)) {
            tok op = ct;
            adv();
            auto right = parse_term();
            node = make_unique<ast::ben>(std::move(node), op.value[0], std::move(right));
        }
        return node;
    }

    unique_ptr<ast::n> parse_term() {
        auto node = parse_fact();
        while (match(token_type::MUL) || match(token_type::DIV)) {
            tok op = ct;
            adv();
            auto right = parse_fact();
            node = make_unique<ast::ben>(std::move(node), op.value[0], std::move(right));
        }
        return node;
    }

    unique_ptr<ast::n> parse_fact() {
        if (ct.type == token_type::INT) {
            int value = stoi(ct.value);
            adv();
            return make_unique<ast::lit>(value);
        } else if (ct.type == token_type::STR) {
            
            string s = ct.value;
            adv();
            return make_unique<ast::lit>(s); 
        } else if (ct.type == token_type::LPAREN) {
            adv();
            auto node = parse_expr();
            consume(token_type::RPAREN);
            return node;
        } else if (ct.type == token_type::IDEF) {
            
            if (ipos + 1 < input.size() && input[ipos+1].type == token_type::LPAREN) {
                
                return parse_call();
            } else {
                string name = ct.value;
                adv();
                return make_unique<ast::ref>(name); 
            }
        } else {
            throw ParseError("Invalid expresion, pos is: "+to_string(ipos));
        }
    }
};
