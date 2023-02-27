#!/bin/sh
export WORKSPACE="$(pwd)"

cd grammar
antlr4 DaoLexer.g4 -package dao -o $WORKSPACE/generated
antlr4 DaoParser.g4 -package dao -o $WORKSPACE/generated -no-listener -visitor
