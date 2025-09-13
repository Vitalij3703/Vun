#pragma once
#include <string>
#include <vector>
#include <variant>
#include <utility>
#include "err.hpp"
// ts looks like big boy code
struct posit {
    int line = 0;
    int col  = 0;
};

namespace ast {
class n {
public:
    std::string    type;
    std::vector<n> children;
    std::string    value;   
    posit          pos;

    n(std::string type,
      std::vector<n> children = {},
      std::string value = "",
      posit pos = {})
    : type(std::move(type)),
      children(std::move(children)),
      value(std::move(value)),
      pos(pos) {}
};


class lit : public n {
public:
    std::variant<int, std::string, bool> literal;

    explicit lit(std::variant<int, std::string, bool> v, posit p = {})
    : n("literal", {}, "", p),
      literal(std::move(v))
    {
        
        if (std::holds_alternative<std::string>(literal)) {
            value = std::get<std::string>(literal);
        } else if (std::holds_alternative<int>(literal)) {
            value = std::to_string(std::get<int>(literal));
        } else {
            value = std::get<bool>(literal) ? "true" : "false";
        }
    }
};


class ben : public n {
public:
    ben(n left, char op, n right, posit p = {})
    : n("binaryop",
        std::vector<n>{ std::move(left), std::move(right) },
        std::string(1, op),
        p)
    {}
};


class fn : public n {
public:
    std::string name;
    std::vector<std::string> params;

    fn(std::string name,
       std::vector<std::string> params = {},
       std::vector<n> body = {},
       posit p = {})
    : n("function", std::move(body), name, p),
      name(std::move(name)),
      params(std::move(params))
    {}
};


class ifn : public n {
public:
    n condition;

    ifn(n condition, std::vector<n> body = {}, posit p = {})
    : n("if", std::move(body), "", p),
      condition(std::move(condition))
    {
        
        children.insert(children.begin(), this->condition);
    }
};


class wn : public n {
public:
    n condition;

    wn(n condition, std::vector<n> body = {}, posit p = {})
    : n("while", std::move(body), "", p),
      condition(std::move(condition))
    {
        children.insert(children.begin(), this->condition);
    }
};


class var : public n {
public:
    
    var(std::string name, n valueExpr, posit p = {})
    : n("var", std::vector<n>{ std::move(valueExpr) }, std::move(name), p)
    {}
};
}