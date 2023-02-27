#include <iostream>
#include <string>

#include <antlr4-runtime.h>

#include "DaoLexer.h"
#include "DaoParser.h"
#include "DaoVisitor.hpp"

using namespace dao;

void print_tokens(const std::string &source_file)
{
  antlr4::ANTLRFileStream fileStream(source_file);

  DaoLexer lexer(&fileStream);
  antlr4::CommonTokenStream tokens(&lexer);

  while (!lexer.hitEOF)
  {
    auto token = lexer.nextToken();
    std::cout << token->getType() << " " << token->getText() << std::endl;
  }
}

void eval(const std::string &source_file)
{
  antlr4::ANTLRFileStream fileStream(source_file);

  DaoLexer lexer(&fileStream);
  antlr4::CommonTokenStream tokens(&lexer);

  dao::DaoParser parser(&tokens);
  auto tree = parser.file_input();

  DaoVisitor visitor;
  visitor.visit(tree);
}

int main()
{
  std::string file_name = "demo/demo04.dao";
  // print_tokens(file_name);
  eval(file_name);

  return 0;
}