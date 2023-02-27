lexer grammar DaoLexer;

options { superClass=DaoBaseLexer; language=Cpp; }


@lexer::header {
#include "DaoBaseLexer.hpp"
#include "DaoParser.h"
#include "Utils.hpp"
}

Import:                          '导入';
                    
Func:                            '函数';
Class:                           '类';
Struct:                          '结构体';
Union:                           '共用体';
Enum:                            '枚举';
                    
If:                              '如果';
ElseIf:                          '或者';
Else:                            '否则';
While:                           '当';
For:                             '对于';
                    
Return:                          '返回';
Continue:                        '跳过';
Break:                           '跳出';
Goto:                            '跳到';
                    
Byte:                            '字节';
Int16:                           '短整数';
Int32:                           '整数';
Int64:                           '长整数';
UInt16:                          '短正整数';
UInt32:                          '正整数';
UInt64:                          '长正整数';
Half:                            '短小数';
Float:                           '小数';
Double:                          '长小数';
Bool:                            '逻辑';
Text:                            '文本';
Char:                            '字符';
                    
True:                            '真';
False:                           '假';
Null:                            '空';
                    
LeftParen:                       '(';
RightParen:                      ')';
LeftBracket:                     '[';
RightBracket:                    ']';
LeftBrace:                       '{';
RightBrace:                      '}';

Less:                            '<';
LessEqual:                       '<=';
Greater:                         '>';
GreaterEqual:                    '>=';
LeftShift:                       '<<';
RightShift:                      '>>';

Plus:                            '+';
PlusPlus:                        '++';
Minus:                           '-';
MinusMinus:                      '--';
Star:                            '*';
Div:                             '/';
Mod:                             '%';

And:                             '&';
Or:                              '|';
AndAnd:                          '&&';
OrOr:                            '||';
Caret:                           '^';
Not:                             '!';
Tilde:                           '~';

Question:                        '?';
Colon:                           ':';
ColonColon:                      '::';
Semi:                            ';';
Comma:                           ',';

Assign:                          '=';
StarAssign:                      '*=';
DivAssign:                       '/=';
ModAssign:                       '%=';
PlusAssign:                      '+=';
MinusAssign:                     '-=';
LeftShiftAssign:                 '<<=';
RightShiftAssign:                '>>=';
AndAssign:                       '&=';
XorAssign:                       '^=';
OrAssign:                        '|=';

Equal:                           '==';
NotEqual:                        '!=';

Arrow:                           '->';
Dot:                             '.';
Ellipsis:                        '...';

Skip:                            (Spaces|Comment|LineJoining) -> skip;
Newline:                         ({atStartOfInput()}?Spaces | ('\r'?'\n'|'\r'|'\f') Spaces?)
   {
     {
         std::string newLine, spaces;
         std::string text = getText();
         for(char c : text)
         {
           if ((c == '\r') || (c == '\n') || (c == '\f'))
             newLine.push_back(c);
           else
             spaces.push_back(c);
         }

         // Strip newlines inside open clauses except if we are near EOF. We keep NEWLINEs near EOF to
         // satisfy the final newline needed by the single_put rule used by the REPL.
         int next = _input->LA(1);
         int nextnext = _input->LA(2);
         if (m_opened > 0 || (nextnext != -1 && (next == '\r' || next == '\n' || next == '\f' || next == '#'))) {
           // If we're inside a list or on a blank line, ignore all indents,
           // dedents and line breaks.
           skip();
         }
         else {
           emit(commonToken(Newline, newLine));
           int indent = get_indentation_count(spaces);
           int previous = m_indents.empty() ? 0 : m_indents.top();
           if (indent == previous) {
             // skip indents of the same size as the present indent-size
             skip();
           }
           else if (indent > previous) {
             m_indents.push(indent);
             emit(commonToken(DaoParser::INDENT, spaces));
           }
           else {
             // Possibly emit more than 1 DEDENT token.
             while(!m_indents.empty() && m_indents.top() > indent) {
               emit(createDedent());
               m_indents.pop();
             }
           }
         }
     }
   }
 ;

fragment Spaces:                 [ \t]+;
fragment Comment:                '#' ~[\r\n\f]*;
fragment LineJoining:           '\\' Spaces? ( '\r'? '\n' | '\r' | '\f');

IntegerLiteral:                  Digit+;
FloatLiteral:                    Digit* '.' Digit+ | Digit+ '.';

fragment Digit:                  [0-9];

StringLiteral:                   '"' SCharSequence? '"';

fragment SCharSequence:          SChar+;
fragment SChar:                  ~'"' | '\\"';

Identifier:                      IdentifierStart IdentifierContinue*;

fragment IdentifierStart
    : [a-zA-Z_]
    | ChineseChar
    ;

fragment IdentifierContinue
    : [a-zA-Z0-9_]
    | ChineseChar
    ;

// 中文字符集 https://www.qqxiuzi.cn/zh/hanzi-unicode-bianma.php
fragment ChineseChar 
    : '\u4E00'..'\u9FA5'
    | '\u9FA6'..'\u9FFF'
    | '\u3400'..'\u4DBF'
    | '\u{20000}'..'\u{2A6DF}'
    | '\u{2A700}'..'\u{2B738}'
    | '\u{2B740}'..'\u{2B81D}'
    | '\u{2B820}'..'\u{2CEA1}'
    | '\u{2CEB0}'..'\u{2EBE0}'
    | '\u{30000}'..'\u{3134A}'
    | '\u2F00'..'\u2FD5'
    | '\u2E80'..'\u2EF3'
    | '\uF900'..'\uFAD9'
    | '\u{2F800}'..'\u{2FA1D}'
    | '\uE815'..'\uE86F'
    | '\uE400'..'\uE5E8'
    | '\uE600'..'\uE6CF'
    | '\u31C0'..'\u31E3'
    | '\u2FF0'..'\u2FFB'
    | '\u3105'..'\u312F'
    | '\u31A0'..'\u31BA'
    | '\u3007'
    ;

UNKNOWN_CHAR:                    .;