// ill expand the parser in the future
#pragma once
#include <iostream>
#include <string>
#include "lexer.hpp"
#include <vector>
#include "err.hpp"
#include <memory>
#include "ast.hpp"
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
        
        if (input.empty()) throw ParseError({}); 
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

    bool consume(token_type expected_token, string expected_char) {
        if (match(expected_token, expected_char)) return true;
        throw ParseError(ct);
    }

    bool consume(token_type expected_token) {
        if (match(expected_token)) return true;
        throw ParseError(ct);
    }
    
    vector<unique_ptr<ast::n>> parse() {
        vector<unique_ptr<ast::n>> nlist;
        while (ct.type != token_type::FE){
            unique_ptr<ast::n> node = parse_stat();
            if (node){
                nlist.push_back(std::move(node));
            } else {
                
                throw ParseError(ct);
            }
        }
        return nlist;
    }

    
    unique_ptr<ast::n> parse_stat() {
        if (match(token_type::KEYW, "int") || match(token_type::KEYW, "str")) {
            adv(); 
            if (match(token_type::IDEF)) {
                string name = ct.value;
                adv(); 
                if (match(token_type::EQUL)) {
                    adv(); 
                    auto expr = parse_expr();
                    return make_unique<ast::var>(name, std::move(expr));
                }
                return make_unique<ast::var>(name, unique_ptr<ast::n>(nullptr));
            }
        }
        else if (match(token_type::IDEF)){
            
            
            return parse_call();
        }
        return parse_expr();
    }

    
    unique_ptr<ast::n> parse_call() {
        
        if (!match(token_type::IDEF)) throw ParseError(ct);
        string func_name = ct.value;
        adv(); 
        consume(token_type::LPAREN);
        adv(); 
        vector<unique_ptr<ast::n>> args;
        if (!match(token_type::RPAREN)) {
            do {
                args.push_back(std::move(parse_expr()));
                if (match(token_type::COMMA)) adv();
                else break;
            } while (true);
        }
        consume(token_type::RPAREN);
        adv();
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
            adv();
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
            throw ParseError(ct);
        }
    }
};
