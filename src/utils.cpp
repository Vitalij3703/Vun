#include "utils.hpp"
#include <cstdint>
#include <variant>
#include <string>
#include "err.hpp"
#include <sstream>
#include <fstream>



Value::Value() : v(std::monostate{}), tag(ValType::VOID) {}
Value::Value(std::int64_t i) : v(i), tag(ValType::INT) {}
Value::Value(double f) : v(f), tag(ValType::FLOAT) {}
Value::Value(const std::string& s) : v(s), tag(ValType::STR) {}
Value::Value(std::string&& s) : v(std::move(s)), tag(ValType::STR) {}
Value::Value(const char* s) : v(std::string(s)), tag(ValType::STR) {}
Value::Value(bool b) : v(b), tag(ValType::BOOL) {}

Value Value::Void() { return Value(); }

bool Value::is_int() const   { return tag == ValType::INT; }
bool Value::is_float() const { return tag == ValType::FLOAT; }
bool Value::is_str() const   { return tag == ValType::STR; }
bool Value::is_bool() const  { return tag == ValType::BOOL; }
bool Value::is_void() const  { return tag == ValType::VOID; }

std::int64_t Value::as_int() const {
    if (is_int()) return std::get<std::int64_t>(v);
    if (is_float()) return static_cast<std::int64_t>(std::get<double>(v));
    if (is_bool()) return std::get<bool>(v) ? 1 : 0;
    throw RuntimeError("Value is not convertible to int");
}

double Value::as_float() const {
    if (is_float()) return std::get<double>(v);
    if (is_int()) return static_cast<double>(std::get<std::int64_t>(v));
    if (is_bool()) return std::get<bool>(v) ? 1.0 : 0.0;
    throw RuntimeError("Value is not convertible to float");
}

const std::string& Value::as_str() const {
    if (is_str()) return std::get<std::string>(v);
    throw RuntimeError("Value is not a string");
}

bool Value::as_bool() const {
    if (is_bool()) return std::get<bool>(v);
    if (is_int()) return std::get<std::int64_t>(v) != 0;
    if (is_float()) return std::get<double>(v) != 0.0;
    throw RuntimeError("Value is not convertible to bool");
}

std::string Value::to_string() const {
    std::ostringstream oss;
    if (is_int()) oss << std::get<std::int64_t>(v);
    else if (is_float()) oss << std::get<double>(v);
    else if (is_str()) oss << std::get<std::string>(v);
    else if (is_bool()) oss << (std::get<bool>(v) ? "true" : "false");
    else oss << "";
    return oss.str();
}


std::string gfc(const string name){
    if (name.size() < 3 || name.substr(name.size() - 3) != ".vl") {
        throw RuntimeError("Wrong file extension in include. Expected .vl");
    }
    std::ifstream file(name);
    if (!file.is_open()) {
        throw RuntimeError("Couldnt open the file: " + name);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}