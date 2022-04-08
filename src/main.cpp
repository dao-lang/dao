#include <iostream>
#include <string>

#include <antlr4-runtime.h>

#include "DaoLexer.h"
#include "DaoParser.h"
#include "DaoVisitor.h"

void print_tokens(const std::string source_file)
{
  antlr4::ANTLRFileStream fileStream;
  fileStream.loadFromFile(source_file);

  DaoLexer lexer(&fileStream);
  antlr4::CommonTokenStream tokens(&lexer);

  while (!lexer.hitEOF)
  {
    auto token = lexer.nextToken();
    std::cout << token->getType() << " " << token->getText() << std::endl;
  }
}

int main()
{
  antlr4::ANTLRFileStream fileStream;
  fileStream.loadFromFile("demo/demo01.dao");

  DaoLexer lexer(&fileStream);
  antlr4::CommonTokenStream tokens(&lexer);

  DaoParser parser(&tokens);
  auto tree = parser.file_input();

  DaoVisitor visitor;
  visitor.visit(tree);

  return 0;
}