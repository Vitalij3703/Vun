// eofhuoluzdyhgfjruujy
#pragma once
#include <cstdint>
#include <variant>
#include <string>
#include "ast.hpp"

struct badidea {
    bool yes;
    badidea(bool yon) : yes(yon){}
};

// The enum for types of values
enum class ValType { INT, FLOAT, STR, BOOL, VOID, B };

// The value struct used for values (obviously)
struct Value {
    std::variant<std::int64_t, double, std::string, bool, std::monostate, badidea> v;
    ValType tag;

    Value();
    Value(std::int64_t i);
    Value(double f);
    Value(const std::string& s);
    Value(std::string&& s);
    Value(const char* s);
    Value(bool b);
    Value(badidea i);
    static Value Void();

    bool is_int() const;
    bool is_float() const;
    bool is_str() const;
    bool is_bool() const;
    bool is_void() const;

    std::int64_t as_int() const;
    double as_float() const;
    const std::string& as_str() const;
    bool as_bool() const;

    std::string to_string() const;
};

// get the contents of a library file
std::string gfc(const string name, bool debug);

void outnode_d(ast::n* node);
std::vector<ast::n*> make_vec(ast::n* what);

// convert escapes to their corresponding characters
std::string cettcc(const std::string& input);