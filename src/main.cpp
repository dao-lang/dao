#include <iostream>
#include <string>

#include <antlr4-runtime.h>

#include "DaoLexer.h"
#include "DaoParser.h"
#include "DaoVisitor.hpp"

using namespace dao;
using namespace antlr4;

void print_tokens(const std::string &source_file) {
    ANTLRFileStream fileStream(source_file);

    DaoLexer lexer(&fileStream);
    CommonTokenStream tokens(&lexer);

    tokens.fill();
    for (Token *token: tokens.getTokens()) {
        std::cout << (int) token->getType() << " " << token->getText() << std::endl;
    }
}

void eval(const std::string &source_file) {
    ANTLRFileStream fileStream(source_file);

    DaoLexer lexer(&fileStream);
    CommonTokenStream tokens(&lexer);

    DaoParser parser(&tokens);
    auto tree = parser.file_input();

    DaoVisitor visitor;
    visitor.visit(tree);
}

int main() {
    std::string file_name = "demo/demo03.dao";
    print_tokens(file_name);
    //eval(file_name);

    return 0;
}