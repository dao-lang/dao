#!/bin/sh
export WORKSPACE="$(pwd)"
export CLASSPATH="$WORKSPACE/tools/antlr-4.9.3-complete.jar:$CLASSPATH"

cd grammar
java org.antlr.v4.Tool DaoLexer.g4 -package dao -o $WORKSPACE/generated 
java org.antlr.v4.Tool DaoParser.g4 -package dao -o $WORKSPACE/generated -no-listener -visitor
