lexer grammar DaoLexer;

options { language=Cpp; }

tokens { Begin, End }

@lexer::members {
private:
    std::vector<std::unique_ptr<antlr4::Token>> m_tokens;
    std::stack<int> m_indents;
    int m_opened = 0;
    std::unique_ptr<antlr4::Token> m_pLastToken = nullptr;

public:
    virtual void emit(std::unique_ptr<antlr4::Token> newToken) override {
        m_tokens.push_back(cloneToken(newToken));
        setToken(std::move(newToken));
    }

    std::unique_ptr<antlr4::Token> nextToken() override {
        // Check if the end-of-file is ahead and there are still some DEDENTS expected.
        if (_input->LA(1) == EOF && !m_indents.empty()) {
            // Remove any trailing EOF tokens from our buffer.
            for (int i = m_tokens.size() - 1; i >= 0; i--) {
                if (m_tokens[i]->getType() == EOF) {
                    m_tokens.erase(m_tokens.begin() + i);
                }
            }

            // First emit an extra line break that serves as the end of the statement.
            emit(commonToken(Newline, "", "<Newline>"));

            // Now emit as much DEDENT tokens as needed.
            while (!m_indents.empty()) {
                emit(createDedent());
                m_indents.pop();
            }

            // Put the EOF back on the token stream.
            emit(commonToken(EOF, "", "<EOF>"));
        }

        std::unique_ptr<antlr4::Token> next = Lexer::nextToken();

        if (next->getChannel() == antlr4::Token::DEFAULT_CHANNEL) {
            // Keep track of the last token on the default channel.
            m_pLastToken = cloneToken(next);
        }

        if (!m_tokens.empty()) {
            next = std::move(*m_tokens.begin());
            m_tokens.erase(m_tokens.begin());
        }

        return next;
    }

private:
    std::unique_ptr<antlr4::Token> createDedent() {
        return commonToken(End, "", "<End>");
    }

    std::unique_ptr<antlr4::CommonToken> commonToken(size_t type, const std::string &text, const std::string &display_text) {
        int stop = getCharIndex() - 1;
        int start = text.empty() ? stop : stop - text.size() + 1;
        return _factory->create({this, _input}, type, display_text, DEFAULT_TOKEN_CHANNEL, start, stop, m_pLastToken ? m_pLastToken->getLine() : 0, m_pLastToken ? m_pLastToken->getCharPositionInLine() : 0);
    }

    std::unique_ptr<antlr4::CommonToken> cloneToken(const std::unique_ptr<antlr4::Token> &source) {
        return _factory->create({this, _input}, source->getType(), source->getText(), source->getChannel(), source->getStartIndex(), source->getStopIndex(), source->getLine(), source->getCharPositionInLine());
    }

    static int getIndentationCount(const std::string &spaces) {
        int count = 0;
        for (char ch: spaces) {
            if (ch == '\t') count += 8 - (count % 8); else count++;
        }
        return count;
    }

    bool atStartOfInput() {
        return getCharPositionInLine() == 0 && getLine() == 1;
    }
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
            if(m_pLastToken && m_pLastToken->getType()!=End)
                emit(commonToken(Newline, newLine, "<Newline>"));
           int indent = getIndentationCount(spaces);
           int previous = m_indents.empty() ? 0 : m_indents.top();
           if (indent == previous) {
             // skip indents of the same size as the present indent-size
             skip();
           }
           else if (indent > previous) {
             m_indents.push(indent);
             emit(commonToken(Begin, spaces, "<Begin>"));
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