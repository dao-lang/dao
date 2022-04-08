parser grammar DaoParser;

options { tokenVocab=DaoLexer; language=Cpp; }

file_input
    : statement*
    ;

statement
    : expression (Newline|EOF)
    ;

expression
    : assignmentExpression (',' assignmentExpression)*
    ;

assignmentExpression
    : conditionalExpression
    | unaryExpression assignmentOperator assignmentExpression
    ;

assignmentOperator
    : '=' | '*=' | '/=' | '%=' | '+=' | '-=' | '<<=' | '>>=' | '&=' | '^=' | '|='
    ;

conditionalExpression
    : logicalOrExpression ('?' expression ':' conditionalExpression)?
    ;

logicalOrExpression
    : logicalAndExprression ( '||' logicalAndExprression)*
    ;

logicalAndExprression
    : inclusiveOrExprression ('&&' inclusiveOrExprression)*
    ;

inclusiveOrExprression
    : exclusiveOrExprression ('|' exclusiveOrExprression)*
    ;

exclusiveOrExprression
    : andExprression ('^' andExprression)*
    ;

andExprression
    : equalityExprression ( '&' equalityExprression)*
    ;

equalityExprression
    :   relationalExprression (('=='| '!=') relationalExprression)*
    ;

relationalExprression
    :   shiftExpression (('<'|'>'|'<='|'>=') shiftExpression)*
    ;

shiftExpression
    :   additiveExpression (('<<'|'>>') additiveExpression)*
    ;

additiveExpression
    :   multiplicativeExpression (('+'|'-') multiplicativeExpression)*
    ;

multiplicativeExpression
    :   castExpression (('*'|'/'|'%') castExpression)*
    ;

castExpression
    :   '(' typeName ')' castExpression
    |   unaryExpression
    ;

unaryExpression
    : ('++'| '--')* (postfixExpression | unaryOperator castExpression)
    ;

unaryOperator
    :   '&' | '*' | '+' | '-' | '~' | '!'
    ;

postfixExpression
    : primaryExpression
    | postfixExpression ('[' expression ']' | '(' argumentExpressionList? ')' | ('.' | '->') identifier | ('++' | '--'))
    ;

primaryExpression
    : identifier
    | StringLiteral
    | IntegerLiteral
    | FloatLiteral
    | True
    | False
    | Null
    | '(' expression ')'
    ;

constantExpression
    : conditionalExpression
    ;

argumentExpressionList
    : assignmentExpression (',' assignmentExpression)*
    ;

identifier
    : Identifier
    ;

dotName
    : identifier ('.' identifier)*
    ;

typeName
    : dotName 
    | Byte
    | Int16
    | Int32
    | Int64
    | UInt16
    | UInt32
    | UInt64
    | Half
    | Float
    | Double
    | Bool
    | Text
    ;

funcName
    : (dotName '::')? identifier
    ;
