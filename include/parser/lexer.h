#pragma once
#include <parser/token.hpp>
#include <common/result.h>

#include <fmt/format.h>

#include <vector>
#include <optional>

namespace parser {

    class ParserError : public util::Error {
    public:
        ParserError(const std::string& message, Location location) : Error(message), m_location(location) {}

        [[nodiscard]] const Location& location() const {
            return m_location;
        }

    private:
        Location m_location;
    };

    class Lexer {
    public:
        Lexer() = default;

        util::Results<std::vector<Token>> lex(const std::string &sourceCode);

    private:
        static inline bool isIdentifierCharacter(char c) {
            return std::isalnum(c) || c == '_';
        }

        static inline bool isIntegerCharacter(char c, int base) {
            switch (base) {
                case 16:
                    return std::isxdigit(c);
                case 10:
                    return std::isdigit(c);
                case 8:
                    return c >= '0' && c <= '7';
                case 2:
                    return c == '0' || c == '1';
                default:
                    return false;
            }
        }

        static inline int characterValue(char c) {
            if (c >= '0' && c <= '9') {
                return c - '0';
            } else if (c >= 'a' && c <= 'f') {
                return c - 'a' + 10;
            } else if (c >= 'A' && c <= 'F') {
                return c - 'A' + 10;
            } else {
                return 0;
            }
        }

        static inline size_t getIntegerLiteralLength(const std::string_view& literal) {
            auto count = literal.find_first_not_of("0123456789ABCDEFabcdef'xXoOpP.uU");
            if (count == std::string_view::npos)
                return literal.size();
            else
                return count;
        }

        template<typename... Args>
        inline void error(fmt::format_string<Args...> fmt, Args&&... args) {
            m_errors.emplace_back(fmt::format(fmt, std::forward<Args>(args)...),
                                  Location{ m_line, m_cursor - m_lineBegin });
        }

        std::optional<char> parseCharacter();
        std::optional<Token> parseOperator();
        std::optional<Token> parseSeparator();
        std::optional<Token> parseKeyword(const std::string &identifier);
        std::optional<Token> parseStringLiteral();
        std::optional<Token::Literal> parseIntegerLiteral(std::string_view literal);

        Token makeToken(const Token& token);

        std::string m_sourceCode;
        std::vector<ParserError> m_errors;
        u32 m_cursor{};
        u32 m_line{};
        u32 m_lineBegin{};
    };

}