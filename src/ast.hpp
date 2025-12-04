#pragma once
#include <string>
#include <vector>
#include <variant>
#include <memory>
#include "err.hpp"
#include <utility>

struct posit {
    int line = 0;
    int col  = 0;
};

namespace ast {
	// node
    class n {
    public:
        std::string type;
        std::vector<std::unique_ptr<n>> children;
        std::string value;
        posit pos;

        n(std::string type,
          std::vector<std::unique_ptr<n>> children = {},
          std::string value = "",
          posit pos = {})
        : type(std::move(type)),
          children(std::move(children)),
          value(std::move(value)),
          pos(pos) {}
        virtual const std::vector<std::string>* get_params() const { return nullptr; }
        virtual const bool is_consta() const { return false; }
        virtual const bool is_str_lit() const { return false; }
        virtual const bool is_int_lit() const { return false; }
        virtual const bool is_bool_lit() const { return false; }
        virtual const bool is_float_lit() const { return false; }
        virtual n* a() { return nullptr; }
    };
	// literal
    class lit : public n {
    public:
        std::variant<int, std::string, bool, double> literal;

        explicit lit(std::variant<int, std::string, bool, double> v, posit p = {})
        : n("literal", {}, "", p),
          literal(std::move(v))
        {
            if (std::holds_alternative<std::string>(literal)) value = std::get<std::string>(literal);
            else if (std::holds_alternative<int>(literal)) value = std::to_string(std::get<int>(literal));
            else if (std::holds_alternative<double>(literal)) value = std::to_string(std::get<double>(literal));
            else value = std::get<bool>(literal) ? "true" : "false";
        }
        const bool is_str_lit() const override {
            if (std::holds_alternative<std::string>(literal)){
                return true;
            }
            return false;
        }
        const bool is_int_lit() const override {
            if (std::holds_alternative<int>(literal)){
                return true;
            }
            return false;
        }
        const bool is_bool_lit() const override {
            if (std::holds_alternative<bool>(literal)){
                return true;
            }
            return false;
        }
        const bool is_float_lit() const override {
            if (std::holds_alternative<double>(literal)){
                return true;
            }
            return false;
        }
    };
	// binary expr
    class ben : public n {
    public:
        ben(std::unique_ptr<n> left, std::string op, std::unique_ptr<n> right, posit p = {})
        : n("binaryop",
            make_vector(std::move(left), std::move(right)),
            op,
            p)
        {}

    private:
        static std::vector<std::unique_ptr<n>> make_vector(std::unique_ptr<n> left, std::unique_ptr<n> right) {
            std::vector<std::unique_ptr<n>> v;
            v.push_back(std::move(left));
            v.push_back(std::move(right));
            return v;
        }
    };
	// function node
    class fn : public n {
    public:
        std::string name;
        std::vector<std::string> params;

        fn(std::string name,
           std::vector<std::string> params = {},
           std::vector<std::unique_ptr<n>> body = {},
           posit p = {})
        : n("function", std::move(body), name, p),
          name(std::move(name)),
          params(std::move(params)) {}
        const std::vector<std::string>* get_params() const override {
            return &params;
        }
    };
	// callable
    class call : public n {
    public:
        std::string fn_name;
        std::vector<std::unique_ptr<n>> args;

        call(std::string fn_name,
             std::vector<std::unique_ptr<n>> args = {},
             posit p = {})
        : n("call", {}, fn_name, p),
          fn_name(std::move(fn_name)),
          args(std::move(args))
        {
            for (auto& arg : this->args) children.push_back(std::move(arg));
        }
    };
	// if
    class ifn : public n {
    public:
        std::unique_ptr<n> condition;
        std::vector<std::unique_ptr<n>> then_nodes;

        ifn(std::unique_ptr<n> condition_, std::vector<std::unique_ptr<n>> then_block, posit p = {})
          : then_nodes(std::move(then_block)), condition(std::move(condition_)),
            n("if", std::move(then_block),std::string(""), p)
        {}
        virtual n* a() {return condition.get();}
        
    };
	// for
    class frn : public n {
    public:
        std::unique_ptr<n> _t;
        std::vector<std::unique_ptr<n>> _b;
        frn(std::unique_ptr<n> times, std::vector<std::unique_ptr<n>> body, posit p = {})
        : _t(std::move(times)), n("for", std::move(body), "", p) {}
        
        virtual n* a() {return _t.get();} // return the times

    };
    // while
    class wn : public n {
    public:
        std::unique_ptr<n> _t;
        std::vector<std::unique_ptr<n>> _b;
        wn(std::unique_ptr<n> cond, std::vector<std::unique_ptr<n>> body, posit p = {})
        : _t(std::move(cond)), n("while", std::move(body), "", p) {}
        
        virtual n* a() {return _t.get();}
    };
	// variable
    class var : public n {
    public:
        bool is_const;
        var(std::string name, std::unique_ptr<n> valueExpr, bool is_const_, posit p = {})
        : n("var", make_vector(std::move(valueExpr)), std::move(name), p), is_const(is_const_)
        {}
        virtual const bool is_consta() const { return is_const; }
    private:
        static std::vector<std::unique_ptr<n>> make_vector(std::unique_ptr<n> value) {
            std::vector<std::unique_ptr<n>> v;
            if (value) v.push_back(std::move(value));
            return v;
        }
    };
    // reference
    class ref : public n {
        public:
            ref(std::string ref, posit p = {})
            : n("ref", {}, ref)
            {}
    };
    // unary
    class unary : public n {
        public:
            std::string op;
            std::unique_ptr<n> right;
            unary(std::string o, std::unique_ptr<n> r, posit p = {})
              : op(std::move(o)),
                right(nullptr),
                n(
                  "unary",
                  [&]()->std::vector<std::unique_ptr<n>> {
                      std::vector<std::unique_ptr<n>> tmp;
                      tmp.push_back(std::move(r));
                      return tmp;
                  }(),
                  op,
                  p)
            {}

    };

} 






