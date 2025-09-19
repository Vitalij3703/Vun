#pragma once
#include <string>
#include <vector>
#include <variant>
#include <memory>
#include "err.hpp"

struct posit {
    int line = 0;
    int col  = 0;
};

namespace ast {

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
};

class lit : public n {
public:
    std::variant<int, std::string, bool> literal;

    explicit lit(std::variant<int, std::string, bool> v, posit p = {})
    : n("literal", {}, "", p),
      literal(std::move(v))
    {
        if (std::holds_alternative<std::string>(literal)) value = std::get<std::string>(literal);
        else if (std::holds_alternative<int>(literal)) value = std::to_string(std::get<int>(literal));
        else value = std::get<bool>(literal) ? "true" : "false";
    }
};

class ben : public n {
public:
    ben(std::unique_ptr<n> left, char op, std::unique_ptr<n> right, posit p = {})
    : n("binaryop",
        make_vector(std::move(left), std::move(right)),
        std::string(1, op),
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
};

class call : public n {
public:
    std::string fn_name;
    std::vector<std::unique_ptr<n>> args;

    call(std::string fn_name,
         std::vector<std::unique_ptr<n>> args = {},
         posit p = {})
    : n("call", {}, "", p),
      fn_name(std::move(fn_name)),
      args(std::move(args))
    {
        for (auto& arg : this->args) children.push_back(std::move(arg));
    }
};

class ifn : public n {
public:
    std::unique_ptr<n> condition;

    ifn(std::unique_ptr<n> condition,
        std::vector<std::unique_ptr<n>> body = {},
        posit p = {})
    : n("if", std::move(body), "", p),
      condition(std::move(condition))
    {
        children.insert(children.begin(), std::move(this->condition));
    }
};

class wn : public n {
public:
    std::unique_ptr<n> condition;

    wn(std::unique_ptr<n> condition,
       std::vector<std::unique_ptr<n>> body = {},
       posit p = {})
    : n("while", std::move(body), "", p),
      condition(std::move(condition))
    {
        children.insert(children.begin(), std::move(this->condition));
    }
};

class var : public n {
public:
    var(std::string name, std::unique_ptr<n> valueExpr, posit p = {})
    : n("var", make_vector(std::move(valueExpr)), std::move(name), p)
    {}

private:
    static std::vector<std::unique_ptr<n>> make_vector(std::unique_ptr<n> value) {
        std::vector<std::unique_ptr<n>> v;
        if (value) v.push_back(std::move(value));
        return v;
    }
};

} 