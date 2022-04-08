export CLASSPATH="./lib/antlr-4.9.3-complete.jar:$CLASSPATH"

java org.antlr.v4.Tool DaoLexer.g4 -o generated 
java org.antlr.v4.Tool DaoParser.g4 -o generated -no-listener -visitor