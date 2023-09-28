#include "parser/lexer.h"
#include "common/string_util.hpp"

#include <optional>

using namespace parser;
using namespace util;

std::optional<char> Lexer::parseCharacter() {
    const char& c = m_sourceCode[m_cursor];
    if (c == '\\') {
        m_cursor++;
        switch (m_sourceCode[m_cursor++]) {
            case 'a':
                return '\a';
            case 'b':
                return '\b';
            case 'f':
                return '\f';
            case 'n':
                return '\n';
            case 't':
                return '\t';
            case 'r':
                return '\r';
            case '0':
                return '\0';
            case '\'':
                return '\'';
            case '\\':
                return '\\';
            case 'x': {
                char hex[3] = { m_sourceCode[m_cursor], m_sourceCode[m_cursor + 1], 0 };
                m_cursor += 2;
                return static_cast<char>(std::stoul(hex, nullptr, 16));
            }
            case 'u': {
                char hex[5] = { m_sourceCode[m_cursor], m_sourceCode[m_cursor + 1], m_sourceCode[m_cursor + 2],
                                m_sourceCode[m_cursor + 3], 0 };
                m_cursor += 4;
                return static_cast<char>(std::stoul(hex, nullptr, 16));
            }
            default:
                this->error("Unknown escape sequence: {}", m_sourceCode[m_cursor]);
                return std::nullopt;
        }
    } else {
        return c;
    }
}

std::optional<Token> Lexer::parseStringLiteral() {
    std::string result;

    m_cursor++;
    while(m_sourceCode[m_cursor] != '\"') {

        auto character = parseCharacter();

        if(character.has_value()) {
            result += character.value();
        } else {
            return std::nullopt;
        }

        if(m_cursor > m_sourceCode.size()) {
            this->error("Unexpected end of string literal");
            return std::nullopt;
        }

    }
    return makeToken(tokens::Literal::makeString(result));
}

std::optional<Token::Literal> Lexer::parseIntegerLiteral(std::string_view literal) {
    // parse a c like numeric literal
    bool floatSuffix = util::string_ends_with_one_of(literal, { "f", "F", "d", "D" });
    bool unsignedSuffix = util::string_ends_with_one_of(literal, { "u", "U" });
    bool isFloat = literal.find('.') != std::string_view::npos
            || (!literal.starts_with("0x") && floatSuffix);
    bool isUnsigned = unsignedSuffix;

    char suffix = 0;
    if(floatSuffix || unsignedSuffix) {
        suffix = literal[literal.size() - 1];
        literal = literal.substr(0, literal.size() - 1);
    }

    if(isFloat) {
        char *end = nullptr;
        double val = std::strtod(literal.data(), &end);

        if(end != literal.data() + literal.size()) {
            this->error("Invalid float literal: {}", literal);
            return std::nullopt;
        }

        switch (suffix) {
            case 'f':
            case 'F':
                return float(val);
            case 'd':
            case 'D':
            default:
                return val;
        }

    } else {
        u8 base = 10;

        u64 value = 0;
        if(literal[0] == '0') {
            bool hasPrefix = true;
            switch (literal[1]) {
                case 'x':
                case 'X':
                    base = 16;
                    break;
                case 'o':
                case 'O':
                    base = 8;
                    break;
                case 'b':
                case 'B':
                    base = 2;
                    break;
                default:
                    hasPrefix = false;
                    break;
            }
            if (hasPrefix) {
                literal = literal.substr(2);
            }
        }

        for (char c : literal) {
            if (!isIntegerCharacter(c, base)) {
                this->error("Invalid integer literal: {}", literal);
                return std::nullopt;
            }
            value = value * base + characterValue(c);
        }

        if(isUnsigned)
            return u64(value);
        else
            return s64(value); // signed cast


    }
}

std::optional<Token> Lexer::parseOperator() {
    auto operators = Token::operators();
    for (int i = 1; i <= tokens::Operator::maxOperatorLength; ++i) {
        auto operatorToken = operators.find(m_sourceCode.substr(m_cursor, i));
        if (operatorToken != operators.end()) {
            m_cursor += i;
            return makeToken(operatorToken->second);
        }
    }
    return std::nullopt;
}

std::optional<Token> Lexer::parseSeparator() {
    auto separators = Token::separators();
    auto separatorToken = separators.find(m_sourceCode[m_cursor]);
    if (separatorToken != separators.end()) {
        m_cursor++;
        return makeToken(separatorToken->second);
    }
    return std::nullopt;
}

std::optional<Token> Lexer::parseKeyword(const std::string &identifier) {
    auto keywords = Token::keywords();
    auto keywordToken = keywords.find(identifier);
    if (keywordToken != keywords.end()) {
        return makeToken(keywordToken->second);
    }
    return std::nullopt;
}

Token Lexer::makeToken(const parser::Token &token) {
    return Token(token.type(), token.value(), { m_line, m_cursor - m_lineBegin });
}

util::Results<std::vector<Token>> Lexer::lex(const std::string &sourceCode) {
    this->m_sourceCode = sourceCode;
    this->m_cursor = 0;
    this->m_line = 1;

    size_t end = this->m_sourceCode.size();

    std::vector<Token> tokens;
    std::vector<Error> errors;

    while(this->m_cursor < end) {
        const char& c = this->m_sourceCode[this->m_cursor];

        if (c == 0) break; // end of string

        if (std::isblank(c) || std::isspace(c)) {
            if(c == '\n') {
                m_line++;
                m_lineBegin = m_cursor;
            }
        }

        auto operatorToken = parseOperator();
        if (operatorToken.has_value()) {
            tokens.emplace_back(operatorToken.value());
            continue;
        }

        auto separatorToken = parseSeparator();
        if (separatorToken.has_value()) {
            tokens.emplace_back(separatorToken.value());
            continue;
        }

        // literals
        if (c == '"') {
            auto string = parseStringLiteral();

            if (string.has_value()) {
                tokens.emplace_back(string.value());
                continue;
            }
        } else if(c == '\'') {
            m_cursor++;
            auto character = parseCharacter();

            if (character.has_value()) {
                if(m_sourceCode[m_cursor] != '\'') {
                    this->error("Expected closing '");
                    continue;
                }

                tokens.emplace_back(tokens::Literal::makeNumeric(character.value()));
                continue;
            }
        }

        if(isIdentifierCharacter(c) && !std::isdigit(c)) {
            std::string identifier;
            while (isIdentifierCharacter(m_sourceCode[m_cursor])) {
                identifier += m_sourceCode[m_cursor++];
            }

            auto keywordToken = parseKeyword(identifier);
            if(keywordToken.has_value()) {
                tokens.emplace_back(keywordToken.value());
                continue;
            }

            tokens.push_back(tokens::Literal::makeIdentifier(identifier));
            continue;
        } else if(std::isdigit(c)) {
            auto literal = &m_sourceCode[m_cursor];
            size_t size = getIntegerLiteralLength(literal);

            auto integer = parseIntegerLiteral({ literal, size });

            if(integer.has_value()) {
                tokens.emplace_back(tokens::Literal::makeNumeric(integer.value()));
                this->m_cursor += size;
                continue;
            }

            this->m_cursor += size;
            continue;
        } else {
            this->error("Unexpected character: {}", c);
        }

        m_cursor++;
    }

    return tokens;
}