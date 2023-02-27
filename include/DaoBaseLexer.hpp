#pragma once

#include "antlr4-runtime.h"
//#include "DaoLexer.h"
#include "DaoParser.h"



namespace dao {
    class DaoBaseLexer : public antlr4::Lexer {
    public:
        DaoBaseLexer(antlr4::CharStream *input) : antlr4::Lexer(input) {}

        virtual std::string getGrammarFileName() const = 0 ;
        virtual const std::vector<std::string>& getRuleNames() const = 0;

        virtual const std::vector<std::string>& getChannelNames() const = 0;
        virtual const std::vector<std::string>& getModeNames() const = 0;
        virtual const std::vector<std::string>& getTokenNames() const = 0; // deprecated, use vocabulary instead
        virtual antlr4::dfa::Vocabulary& getVocabulary() const = 0;

        virtual const std::vector<uint16_t> getSerializedATN() const = 0;
        virtual const antlr4::atn::ATN& getATN() const = 0;

        virtual void action(antlr4::RuleContext *context, size_t ruleIndex, size_t actionIndex) = 0;
        virtual bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) = 0;

    protected:
        // A queue where extra tokens are pushed on (see the Newline lexer rule).
        std::vector<std::unique_ptr<antlr4::Token>> m_tokens;
        // The stack that keeps track of the indentation level.
        std::stack<int> m_indents;
        // The amount of opened braces, brackets and parenthesis.
        int m_opened = 0;
        // The most recently produced token.
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
                emit(commonToken(DaoParser::Newline, "\n"));

                // Now emit as much DEDENT tokens as needed.
                while (!m_indents.empty()) {
                    emit(createDedent());
                    m_indents.pop();
                }

                // Put the EOF back on the token stream.
                emit(commonToken(EOF, "<EOF>"));
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

    protected:
        std::unique_ptr<antlr4::Token> createDedent() {
            return commonToken(DaoParser::DEDENT, "");
        }

        std::unique_ptr<antlr4::CommonToken> commonToken(size_t type, const std::string &text) {
            int stop = getCharIndex() - 1;
            int start = text.empty() ? stop : stop - text.size() + 1;
            return _factory->create({this, _input}, type, text, DEFAULT_TOKEN_CHANNEL, start, stop, m_pLastToken ? m_pLastToken->getLine() : 0, m_pLastToken ? m_pLastToken->getCharPositionInLine() : 0);
        }

        std::unique_ptr<antlr4::CommonToken> cloneToken(const std::unique_ptr<antlr4::Token> &source) {
            return _factory->create({this, _input}, source->getType(), source->getText(), source->getChannel(), source->getStartIndex(), source->getStopIndex(), source->getLine(), source->getCharPositionInLine());
        }

        bool atStartOfInput() {
            return getCharPositionInLine() == 0 && getLine() == 1;
        }
    };
}