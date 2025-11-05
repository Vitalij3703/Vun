// Vun api
#pragma once
#include <cstdint>
#include <variant>
#include <string>
#include "ast.hpp"

// The enum for types of values
enum class ValType { INT, FLOAT, STR, BOOL, VOID };

// The value struct used for values (obviously)

struct Value {
    std::variant<std::int64_t, double, std::string, bool, std::monostate> v;
    ValType tag;

    Value();
    Value(std::int64_t i);
    Value(double f);
    Value(const std::string& s);
    Value(std::string&& s);
    Value(const char* s);
    Value(bool b);
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

// get the code of a file
std::string gfc(const string name);