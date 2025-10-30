
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
static string token_type_name(token_type t) {
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
        case GRRT: return "GRRT";
        case LWR: return "LWR";
        case LOE: return "LOE";
        case GOE: return "GOE";
        case NOT: return "NOT";
        case OR: return "OR";
        case AND: return "AND";
        case LOUD: return "LOUD";
        case NEQUL: return "NEQUL";
        default: return "NULL";
    }
}

class parser {
private:
    vector<tok> input;
    size_t ipos = 0;
    tok ct;
    string src;

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
        ostringstream oss;
        oss << "expected token (" << token_type_name(expected_token)
            << " / \"" << expected_char << "\") but got (" << token_type_name(ct.type)
            << " / \"" << ct.value << "\") at ipos: " << ipos<<"\n";
        if (!src.empty()) {
            oss << "\n";
            size_t pos = ipos < src.size() ? ipos : src.size();
            size_t start = pos > 30 ? pos - 30 : 0;
            size_t end = min(src.size(), pos + 30);
            oss << "ref: \"" << src.substr(start, end - start) << "\"\n";
            oss << string((size_t) (ipos - start), ' ') << "^\n";
        }
        throw ParseError(oss.str());
    }

    bool consume(token_type expected_token) {
        if (match(expected_token)) { adv(); return true; }
        ostringstream oss;
        oss << "expected token (" << token_type_name(expected_token)
            << ") but got (" << token_type_name(ct.type) << " / \"" << ct.value << "\") at ipos: " << ipos<<"\n";
        if (!src.empty()) {
            oss << "\n";
            size_t pos = ipos < src.size() ? ipos : src.size();
            size_t start = pos > 30 ? pos - 30 : 0;
            size_t end = min(src.size(), pos + 30);
            oss << "ref: \"" << src.substr(start, end - start) << "\"\n";
            oss << string((size_t) (ipos - start), ' ') << "^\n";
        }
        throw ParseError(oss.str());
    }



    vector<unique_ptr<ast::n>> parse() {
        vector<unique_ptr<ast::n>> nlist;
        while (ct.type != token_type::FE){
            unique_ptr<ast::n> node = parse_stat();
            if (node){
                nlist.push_back(move(node));
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
                    return make_unique<ast::var>(name, move(expr), t);
                }
                consume(token_type::SEMI);
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
        else if (match(token_type::KEYW, "if")) return parse_if();
        {
            auto e = parse_expr();
            consume(token_type::SEMI);
        return e;
        }
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
        return make_unique<ast::fn>(name, move(params), move(body));
    }
    unique_ptr<ast::n> parse_fuck() {
        consume(token_type::KEYW, "return");
        auto expr = parse_expr();
        consume(token_type::SEMI);
        return make_unique<ast::n>("return", make_vector(move(expr)));
    }
    unique_ptr<ast::n> parse_for() {
        consume(token_type::KEYW, "for");
        consume(token_type::LPAREN);
        unique_ptr<ast::n> times = parse_expr();
        consume(token_type::RPAREN);
        consume(token_type::LBRACE);

        vector<unique_ptr<ast::n>> body;
        while (!match(token_type::RBRACE)) {
            auto stmt = parse_stat();
            if (!stmt) throw ParseError("invalid statement inside for body");
            body.push_back(move(stmt));
        }
        consume(token_type::RBRACE);
        return make_unique<ast::frn>(move(times), move(body));
    }

    unique_ptr<ast::n> parse_if() {
        consume(token_type::KEYW, "if");
        consume(token_type::LPAREN);
        auto condition = parse_expr();
        consume(token_type::RPAREN);
        consume(token_type::LBRACE);
        vector<unique_ptr<ast::n>> b{};
        while(!match(token_type::RBRACE)){
            b.push_back(parse_stat());
        }
        consume(token_type::RBRACE);
        return make_unique<ast::ifn>(std::move(condition), std::move(b));
    }

    static vector<unique_ptr<ast::n>> make_vector(unique_ptr<ast::n> node) {
        vector<unique_ptr<ast::n>> v;
        v.push_back(move(node));
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
        return make_unique<ast::call>(func_name, move(args));
    }
    
    // top level
    unique_ptr<ast::n> parse_expr() {
        return parse_or();
    }

    unique_ptr<ast::n> parse_or() {
        auto node = parse_and();
        while (match(token_type::OR)) {
            tok op = ct;
            adv();
            auto right = parse_and();
            node = make_unique<ast::ben>(move(node), op.value, move(right));
        }
        return node;
    }

    // AND
    unique_ptr<ast::n> parse_and() {
        auto node = parse_equality();
        while (match(token_type::AND)) {
            tok op = ct;
            adv();
            auto right = parse_equality();
            node = make_unique<ast::ben>(move(node), op.value, move(right));
        }
        return node;
    }

    // EQUL == !=
    unique_ptr<ast::n> parse_equality() {
        auto node = parse_comparison();
        while (match(token_type::EQUL) || match(token_type::NEQUL)) {
            tok op = ct;
            adv();
            auto right = parse_comparison();
            node = make_unique<ast::ben>(move(node), op.value, move(right));
        }
        return node;
    }

    // comp > >= < <=
    unique_ptr<ast::n> parse_comparison() {
        auto node = parse_term();
        while (match(token_type::LWR) || match(token_type::LOE) ||
               match(token_type::GRRT) || match(token_type::GOE)) {
            tok op = ct;
            adv();
            auto right = parse_term();
            node = make_unique<ast::ben>(move(node), op.value, move(right));
        }
        return node;
    }

    unique_ptr<ast::n> parse_term() {
        auto node = parse_fact();
        while (match(token_type::PLU) || match(token_type::MIN)) {
            tok op = ct;
            adv();
            auto right = parse_fact();
            node = make_unique<ast::ben>(move(node), op.value, move(right));
        }
        return node;
    }

    unique_ptr<ast::n> parse_fact() {
        auto node = parse_unary();
        while (match(token_type::MUL) || match(token_type::DIV)) {
            tok op = ct;
            adv();
            auto right = parse_unary();
            node = make_unique<ast::ben>(move(node), op.value, move(right));
        }
        return node;
    }

    // unary operators: '!' and unary '-'
    unique_ptr<ast::n> parse_unary() {
        if (match(token_type::LOUD) || (match(token_type::MIN) && /* disambiguate unary minus? */ true)) {
            tok op = ct;
            adv();
            auto right = parse_unary();
            return make_unique<ast::unary>(op.value, std::move(right), posit(0,0));
        }
        return parse_primary();
    }

    unique_ptr<ast::n> parse_primary() {
        if (ct.type == token_type::INT) {
            int value = stoi(ct.value);
            adv();
            return make_unique<ast::lit>(value);
        } else if (ct.type == token_type::STR) {
            string s = ct.value;
            adv();
            return make_unique<ast::lit>(s); 
        } else if (match(token_type::KEYW, "true")) {
            adv();
            return make_unique<ast::lit>(true);
        } else if (match(token_type::KEYW, "false")) {
            adv();
            return make_unique<ast::lit>(false);
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
            throw ParseError("unexcepted tok");
        }
    }

};
