cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 14)

include_directories(/usr/local/include/antlr4-runtime)
link_directories(/usr/local/lib/)

include_directories(generated include)

add_executable(dao main.cpp generated/DaoLexer.cpp generated/DaoParser.cpp generated/DaoParserVisitor.cpp generated/DaoParserBaseVisitor.cpp src/DaoVisitor.cpp)
target_link_libraries(dao antlr4-runtime)