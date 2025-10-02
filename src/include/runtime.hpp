#include "parser.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include "ast.hpp"

class runtime {
    std::vector<std::unique_ptr<ast::n>> nodes;

public:
    runtime(const std::string& in) {
        parser p(in);
        nodes = p.parse();
        std::cout << "[DEBUG] parsed nodes count = " << nodes.size() << "\n";
    }

void run() {
    if (nodes.empty()) {
        std::cout << "[DEBUG] no nodes — nothing to run\n";
        return;
    }
    for (const auto& node : nodes) {
        if (!node) {
            std::cout << "[DEBUG] null node\n";
            continue;
        }
        std::cout << "[DEBUG] node.type = \"" << node->type
                  << "\" node.value = \"" << node->value << "\"\n";
        if (node->type == "var") {
            std::string varname = node->value;
            if (!node->children.empty() && node->children[0]) {
                std::string assigned = node->children[0]->value;
                std::cout << "[DEBUG] assigned var '" << varname
                          << "' with the value \"" << assigned << "\"\n";
            } else {
                std::cout << "[DEBUG] var '" << varname
                          << "' declared without initializer\n";
            }
        }
        if (node->type == "literal") {
            std::cout << "[DEBUG] literal value = \"" << node->value << "\"\n";
        }
    }
}
};
