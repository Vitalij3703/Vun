
#pragma once
#include "parser.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include "ast.hpp"
#include "err.hpp"
#include <unordered_map>
#include <optional>
#include <sstream>
#include <variant>
#include <cstdint>
#include <algorithm>
#include "utils.hpp"
#include <filesystem>




const std::string OPS[4] = {"+", "-", "*", "/"}; // arithmetic operators

bool debug = false;

// apply arithmetics to a and b with the operator op
Value apply_arith(const Value& a, const Value& b, char op) {
    if (op == '+' && (a.is_str() || b.is_str())) {
        std::string left = a.is_str() ? a.as_str() : a.to_string();
        std::string right = b.is_str() ? b.as_str() : b.to_string();
        return Value(left + right);
    }

    bool promotes_to_float = (a.is_float() || b.is_float() || op == '/');

    if (promotes_to_float) {
        float af = a.as_float();
        float bf = b.as_float();
        float res;
        switch (op) {
            case '+': res = af + bf; break;
            case '-': res = af - bf; break;
            case '*': res = af * bf; break;
            case '/':
                if (bf == 0.0) throw RuntimeError("Division by zero");
                res = af / bf; break;
            
            default: throw RuntimeError(std::string("Unknown binary operator: ") + op);
        }
        return Value(res);
    } else {
        std::int64_t ai = a.as_int();
        std::int64_t bi = b.as_int();
        switch (op) {
            case '+': return Value(ai + bi);
            case '-': return Value(ai - bi);
            case '*': return Value(ai * bi);
            case '/':
                if (bi == 0) throw RuntimeError("Division by zero");
                return Value(ai / bi);
            case '%':
                return Value(ai%bi);
            default: throw RuntimeError(std::string("Unknown binary operator: ") + op);
        }
    }
}

bool isTruthy(const Value& v) {
    if (v.is_void()) return false;
    if (v.is_bool()) return v.as_bool();
    if (v.is_int()) return v.as_int() != 0;
    if (v.is_float()) return v.as_float() != 0.0;
    if (v.is_str()) return !v.as_str().empty();
    return false;
}

bool equalValues(const Value& a, const Value& b) {
    if (a.tag == b.tag) {
        if (a.is_int()) return a.as_int() == b.as_int();
        if (a.is_float()) return a.as_float() == b.as_float();
        if (a.is_bool()) return a.as_bool() == b.as_bool();
        if (a.is_str()) return a.as_str() == b.as_str();
        if (a.is_void()) return true;
    }
    if ((a.is_int() && b.is_float()) || (a.is_float() && b.is_int())) {
        return a.as_float() == b.as_float();
    }
    if (a.is_bool() && (b.is_int() || b.is_float())) {
        return (a.as_bool() ? 1 : 0) == b.as_int();
    }
    if (b.is_bool() && (a.is_int() || a.is_float())) {
        return (b.as_bool() ? 1 : 0) == a.as_int();
    }
    return false;
}

int compareValues(const Value& a, const Value& b) {
    if (a.is_int() && b.is_int()) {
        auto ad = a.as_int();
        auto bd = b.as_int();
        if (ad < bd) return -1;
        if (ad > bd) return 1;
        return 0;
    }
    if (a.is_float() && b.is_float()) {
        auto ad = a.as_float();
        auto bd = b.as_float();
        if (ad < bd) return -1;
        if (ad > bd) return 1;
        return 0;
    }
    if (a.is_str() && b.is_str()) {
        if (a.as_str() < b.as_str()) return -1;
        if (a.as_str() > b.as_str()) return 1;
        return 0;
    }
    if (a.is_bool() && b.is_bool()) {
        bool ad = a.as_bool();
        bool bd = b.as_bool();
        if (ad == bd) return 0;
        return ad ? 1 : -1;
    }

    if ((a.is_int() && b.is_float()) || (a.is_float() && b.is_int())) {
        double ad = a.is_int() ? static_cast<double>(a.as_int()) : a.as_float();
        double bd = b.is_int() ? static_cast<double>(b.as_int()) : b.as_float();
        if (ad < bd) return -1;
        if (ad > bd) return 1;
        return 0;
    }

    if (a.is_bool() && (b.is_int() || b.is_float())) {
        double ad = a.as_bool() ? 1.0 : 0.0;
        double bd = b.is_int() ? static_cast<double>(b.as_int()) : b.as_float();
        if (ad < bd) return -1;
        if (ad > bd) return 1;
        return 0;
    }
    if (b.is_bool() && (a.is_int() || a.is_float())) {
        double bd = b.as_bool() ? 1.0 : 0.0;
        double ad = a.is_int() ? static_cast<double>(a.as_int()) : a.as_float();
        if (ad < bd) return -1;
        if (ad > bd) return 1;
        return 0;
    }

    throw RuntimeError("Cannot compare incompatible types");
}

Value apply_binary(const Value& left, const Value& right, const std::string& op) {
    if (op == "|") {
        return Value(isTruthy(left) ? left : right);
    }
    if (op == "&") {
        return Value(isTruthy(left) ? right : left);
    }
    if (op == "==" || op == "!=") {
        bool eq = equalValues(left, right);
        return Value(op == "==" ? eq : !eq);
    }
    if (op == "<" || op == ">" || op == "<=" || op == ">=") {
        int cmp = compareValues(left, right);
        if (op == "<") return Value(cmp < 0);
        if (op == ">") return Value(cmp > 0);
        if (op == "<=") return Value(cmp <= 0);
        if (op == ">=") return Value(cmp >= 0);
    }

    if (op.size() == 1) {
        char c = op[0];
        if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%') {
            return apply_arith(left, right, c);
        }
    }

    throw RuntimeError(std::string("Unsupported operator: ") + op);
}





class runtime {
    
public:
    std::vector<std::unique_ptr<ast::n>> nodesl;                         // stores nodes
    std::vector<std::unique_ptr<ast::n>> global_nodes;                   // stores whatever, if it works it works
    std::unordered_map<std::string, Value> vars;                         // stores variables
    std::unordered_map<std::string, ast::fn*> functions;                 // stores functions
    std::string strbuf = "";                                             // stores the strbuf
    std::string fnamee;


    runtime(const std::string& in, std::string fabspath) {
        fnamee = fabspath;
        parser p(in, debug);
        if (debug) std::cout << "[DEBUG] input size: " << in.size() << "\n";
        nodesl = p.parse();
        if (debug) std::cout << "[DEBUG] parsed nodes count: " << nodesl.size() << "\n";

        if (debug){
            for(auto& node : nodesl) outnode_d(node.get());
        }

        if(!nodesl.empty()){
            auto top_ret = run(nodesl, false);
            if (top_ret) {
                auto v = top_ret.value();
                throw RuntimeInfo("Program returned "+v.to_string());
            }
        }
    }

    Value evaluateExpression(ast::n* node) {
    if (!node) throw RuntimeError("Null node in eval");

    if (node->type == "literal") {
        auto* litnode = dynamic_cast<ast::lit*>(node);
        if (!litnode) throw ParseError("Literal node cast failed");

        if (std::holds_alternative<int>(litnode->literal)) {
            int v = std::get<int>(litnode->literal);
            return Value(static_cast<std::int64_t>(v));
        } else if (std::holds_alternative<std::string>(litnode->literal)) {
            return Value(std::get<std::string>(litnode->literal));
        } else if (std::holds_alternative<bool>(litnode->literal)) {
            return Value(std::get<bool>(litnode->literal));
        } else if (std::holds_alternative<double>(litnode->literal)){
            return Value(std::get<double>(litnode->literal));
        } else {
            return Value::Void();
        }
    }

    // handle binary operation
    if (node->type == "binaryop") {
        if (node->children.size() < 2) throw ParseError("Binary op missing operand");

        std::string op = node->value;

        if (op == "|") {
            Value left = evaluateExpression(node->children[0].get());
            if (isTruthy(left)) return left;
            return evaluateExpression(node->children[1].get());
        }
        if (op == "&") {
            Value left = evaluateExpression(node->children[0].get());
            if (!isTruthy(left)) return left;
            return evaluateExpression(node->children[1].get());
        }

        Value left = evaluateExpression(node->children[0].get());
        Value right = evaluateExpression(node->children[1].get());
        return apply_binary(left, right, op);
    }

    // unary operators
    if (node->type == "unary") {
        if (node->children.empty() || !node->children[0]) throw ParseError("Unary missing operand");
        std::string op = node->value;
        Value val = evaluateExpression(node->children[0].get());
        if (op == "!") {
            return Value(!isTruthy(val));
        } else if (op == "-" || op == "u-") {
            if (val.is_int()) return Value(-val.as_int());
            if (val.is_float()) return Value(-val.as_float());
            throw RuntimeError("Unary '-' on non-numeric type");
        }
        throw RuntimeError("Unknown unary operator: " + op);
    }

    if (node->type == "ref") {
        auto it = vars.find(node->value);
        if (it == vars.end()) throw RuntimeError("Unknown variable: " + node->value);
        if(it->second.tag == ValType::VOID){
            throw RuntimeError("Variable '"+node->value+"' doesnt have a value yet it got referenced");
        }
        return it->second;
    }

    if (node->type == "call") {
        std::vector<Value> arg_vals;
        for (const auto& child : node->children) arg_vals.push_back(evaluateExpression(child.get()));

        const std::string call_name = node->value;
        if(debug) std::cout<<"[DEBUG] tryna call function called "+call_name+"\n";
        if (call_name == "isset") {
            if (node->children.size() < 1) throw RuntimeError("isset expects 1 argument");
            ast::n* first_child = node->children[0].get();
            if (first_child->type == "ref") {
                const std::string& name = first_child->value;
                return vars.find(name) != vars.end();
            } else if (!arg_vals.empty()) {
                if (arg_vals[0].is_str()) {
                    return vars.find(arg_vals[0].as_str()) != vars.end();
                }
            }
            return false;
        }
        auto it_fn = functions.find(call_name);
        if (it_fn != functions.end()) {
            ast::n* fn_node = it_fn->second;
            auto params_ptr = fn_node->get_params();
            if (!params_ptr) throw RuntimeError("Function has no params container");
            auto& params = *params_ptr;
            if (params.size() != arg_vals.size()) throw RuntimeError("Argument count mismatch when calling function '" + call_name + "'");

            auto old_vars = vars;
            auto old_functions = functions;

            for (size_t i = 0; i < params.size(); ++i) {
                const std::string& param_name = params[i].second;
                vars.insert_or_assign(param_name, arg_vals[i]);
            }

            auto ret = run(fn_node->children,false);

            std::optional<Value> ret_copy;
            if (ret) ret_copy = *ret;
            vars = std::move(old_vars);
            functions = old_functions;
            if (ret_copy) return *ret_copy;
            return Value::Void();
        }

        return Value::Void();
    }

    throw RuntimeError("Unsupported node type in expression: " + node->type);
}

std::optional<Value> run(std::vector<std::unique_ptr<ast::n>>& nodes, bool inc) {
    if (nodes.empty()) {
        if (debug) std::cout << "[DEBUG] input empty\n";
        return std::nullopt;
    }
    std::size_t i = 0;
    for (auto& nde : nodes) {
        i++;
        ast::n* node = nde.get();
        if(inc) global_nodes.push_back(std::move(nde));
        if (!node) continue;

        if (debug) std::cout << "[DEBUG] node.type: \"" << node->type << "\" node.value: \"" << node->value << "\"\n";

        

        if (node->type == "var") {
            std::string varname = node->value;
            if (!node->children.empty() && node->children[0]) {
                Value assigned = evaluateExpression(node->children[0].get());
                vars.insert_or_assign(varname, assigned);
                if (debug) std::cout << "[DEBUG] assigned var '" << varname << "' = " << assigned.to_string() << "\n";
            }
            continue;
        }

        if (node->type == "function") {
            if (debug) std::cout<<"[DEBUG] processing function \""<< node->value <<"\""<<"\n";
            functions.insert_or_assign(node->value, dynamic_cast<ast::fn*>(node));
            continue;
        }

        if (node->type == "return") {
            if (node->children.empty() || !node->children[0]) {
                return std::make_optional(Value::Void());
            } else {
                Value ret = evaluateExpression(node->children[0].get());
                if (debug) std::cout << "[DEBUG] return value: " << ret.to_string() << "\n";
                return std::make_optional(ret);
            }
        }

        if (node->type == "call") {
            if (debug) std::cout << "[DEBUG] calling: " << node->value
                    << " function count=" << (functions.count(node->value) ? "1" : "0")
                    << " children=" << node->children.size() << "\n";

            const std::string call_name = node->value;

            if (call_name == "break"){
                return Value(badidea(true));
            }

            if (call_name == "print") {
                std::cout<<evaluateExpression(node->children[0].get()).to_string();
                continue;
            }
            if (call_name == "println") {
                std::cout<<evaluateExpression(node->children[0].get()).to_string()<<std::endl;
                continue;
            }
            if (call_name == "error") {
                std::cerr<<evaluateExpression(node->children[0].get()).to_string();
                continue;
            }
            if (call_name == "errorln") {
                std::cerr<<evaluateExpression(node->children[0].get()).to_string()<<std::endl;
                continue;
            }
            if (call_name == "tdebug") {
                std::cout << "[DEBUG] Debugging mode on.\n";
                debug = !debug;
                continue;
            }
            if (call_name == "eval"){
                run(node->children, false);
                continue;
            }
            if (call_name == "strbuf_add"){
                Value arg = evaluateExpression(node->children[0].get());
                strbuf += arg.to_string();
                continue;
            }
            if (call_name == "strbuf_outln"){
                std::cout << strbuf<<"\n";
                continue;
            }
            if (call_name == "strbuf_out"){
                std::cout << strbuf;
                continue;
            }
            if (call_name == "include"){
                auto what = evaluateExpression(node->children[0].get()).to_string();
                std::string wha = what;
                if(what.starts_with("std.")){
                    std::filesystem::path p = std::filesystem::absolute(__FILE__).parent_path().parent_path()/"std"/what;
                    wha = gfc(p.string(), debug);
                }
                else {wha = gfc(what, debug);}
                auto a = parser(wha, debug).parse();
                run(a, true);
                continue;
            }
            Value v = evaluateExpression(node);
            continue;
        }
        if (node->type == "for"){
            if (!node->a()) throw RuntimeError("For statement missing iteration expression");
            Value times = evaluateExpression(node->a());
            auto intv = times.as_int();
            auto &body = node->children;
            if(!body.empty()){
            for(int i = 0; i<intv; i++){
                auto r = run(body, false);
                if(r.has_value() && r->tag == ValType::B){
                    break;
                } else if(r.has_value()) {
                    return r;
                }
            }}
            continue;
        }
        if (node->type == "if") {
            if (!node->children[0].get()) throw RuntimeError("If statement missing condition");
            if(debug) std::cout<<"[DEBUG] registering if statament: before condition\n";
            Value cond = evaluateExpression(node->a());
            if(debug) std::cout<<"[DEBUG] registering if statament: after condition\n";
            if (cond.as_bool()) {
                if(!node->children.empty()){
                    run(node->children, false);}
                }
            continue;
        }
    }

    return std::nullopt;
}


}; // end runtime