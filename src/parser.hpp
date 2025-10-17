
#pragma once
#include <iostream>
#include <string>
#include "lexer.hpp"
#include <vector>
#include "err.hpp"
#include <memory>
#include "ast.hpp"
#include <utility>
#include <sstream>
using namespace std;

// helper to print token enum names
static std::string token_type_name(token_type t) {
    switch (t) {
        case IDEF: return "IDEF";
        case LPAREN: return "LPAREN";
        case RPAREN: return "RPAREN";
        case LBRACE: return "LBRACE";
        case RBRACE: return "RBRACE";
        case KEYW: return "KEYW";
        case STR: return "STR";
        case INT: return "INT";
        case DOT: return "DOT";
        case COMMA: return "COMMA";
        case EQUL: return "EQUL";
        case IS: return "IS";
        case DIV: return "DIV";
        case MUL: return "MUL";
        case MIN: return "MIN";
        case PLU: return "PLU";
        case SEMI: return "SEMI";
        case FE: return "FE";
        default: return "NULL";
    }
}

class parser {
private:
    vector<tok> input;
    size_t ipos = 0;
    tok ct;
    std::string src;

public:
    
    parser(string in):src(in) {
        lexer l = lexer(in);
        this->input = l.tokenize();
        /*cout << "[-----Token dump-----]\n";
        for(int i=0;i<size(input); i++){
            cout << to_string(i) <<": "<<token_type_name(input[i].type)<<"("<<input[i].value<<")"<<"\n";
        }*/


        if (input.empty()) throw ParseError("Either, input empty or the lexer's at failure."); 
        ct = input[ipos];
    }

    void adv() {
        ipos++;
        if (ipos < input.size()) {
            ct = input.at(ipos);
        } else {
            ct = tok();
            ct.type = token_type::FE;
            ct.value = "";
        }
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

    bool consume(token_type expected_token, const string& expected_char) {
        if (match(expected_token, expected_char)) { adv(); return true; }
        std::ostringstream oss;
        oss << "expected token (" << token_type_name(expected_token)
            << " / \"" << expected_char << "\") but got (" << token_type_name(ct.type)
            << " / \"" << ct.value << "\") at ipos: " << ipos<<"\n";
        if (!src.empty()) {
            oss << "\n";
            size_t pos = ipos < src.size() ? ipos : src.size();
            size_t start = pos > 30 ? pos - 30 : 0;
            size_t end = std::min(src.size(), pos + 30);
            oss << "reference: \"" << src.substr(start, end - start) << "\"\n";
            oss << std::string((size_t) (ipos - start), ' ') << "^\n";
        }
        throw ParseError(oss.str());
    }

    bool consume(token_type expected_token) {
        if (match(expected_token)) { adv(); return true; }
        std::ostringstream oss;
        oss << "expected token (" << token_type_name(expected_token)
            << ") but got (" << token_type_name(ct.type) << " / \"" << ct.value << "\") at ipos: " << ipos<<"\n";
        if (!src.empty()) {
            oss << "\n";
            size_t pos = ipos < src.size() ? ipos : src.size();
            size_t start = pos > 30 ? pos - 30 : 0;
            size_t end = std::min(src.size(), pos + 30);
            oss << "reference: \"" << src.substr(start, end - start) << "\"\n";
            oss << std::string((size_t) (ipos - start), ' ') << "^\n";
        }
        throw ParseError(oss.str());
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
            auto call = parse_call();
            consume(token_type::SEMI);
            return call;
        }
        else if (match(token_type::KEYW, "func")){
            return parse_func();
        }
        else if (match(token_type::KEYW, "return")) return parse_fuck();
        else if (match(token_type::KEYW, "for")) return parse_for();
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
    std::unique_ptr<ast::n> parse_for() {
        consume(token_type::KEYW, "for");
        consume(token_type::LPAREN);
        std::unique_ptr<ast::n> times;
        if (match(token_type::INT)) {
            auto tmp = std::make_unique<ast::lit>(stoi(ct.value));
            adv();
            times = std::unique_ptr<ast::n>(std::move(tmp));
        }
        else if (match(token_type::IDEF)) {
            auto tmp = std::make_unique<ast::ref>(ct.value);
            adv();
            times = std::unique_ptr<ast::n>(std::move(tmp));
        }
        else {
            throw ParseError("expected integer or identifier in for(...)");
        }

        consume(token_type::RPAREN);
        consume(token_type::LBRACE);

        std::vector<std::unique_ptr<ast::n>> body;
        while (!match(token_type::RBRACE)) {
            auto stmt = parse_stat();
            if (!stmt) throw ParseError("invalid statement inside for body");
            body.push_back(std::move(stmt));
        }
        consume(token_type::RBRACE);
        return std::make_unique<ast::frn>(std::move(times), std::move(body));
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
                args.push_back(parse_expr());
                if (match(token_type::COMMA)) {adv(); continue;}
                else break;
            } while (true);
        }
        consume(token_type::RPAREN);
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
