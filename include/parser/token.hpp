#pragma once

#include <common/types.h>
#include <variant>
#include <string>
#include <map>

namespace parser {

    struct Location {
        u32 line;
        u32 column;
    };

    class Token {
    public:

        enum class Type : u8 {
            Keyword,
            Identifier,
            Operator,
            Separator,
            String,
            Integer
        };

        enum class Keyword : u8 {
            If,
            Else,
            Return,
            Func
        };

        enum class Operator : u8 {
            Plus,
            Minus,
            Multiply,
            Divide,
            Assign,
        };

        enum class Separator : u8 {
            LeftParenthesis,
            RightParenthesis,
            LeftBrace,
            RightBrace,
            LeftBracket,
            RightBracket,
            Comma,
            Dot,
            Semicolon,
            EndOfProgram
        };

        struct Identifier {
            explicit Identifier(std::string identifier) : m_identifier(std::move(identifier)) { }

            [[nodiscard]] const std::string &get() const { return this->m_identifier; }

            bool operator==(const Identifier &) const  = default;

        private:
            std::string m_identifier;
        };

        using Literal = std::variant<std::string, s64, u64, f64>;

        using Value = std::variant<Keyword, Identifier, Operator, Separator, Literal>;

        Token(Type type, Value value, Location location) : m_type(type), m_value(std::move(value)), m_location(location) { }

        [[nodiscard]] inline Type type() const {
            return this->m_type;
        }

        [[nodiscard]] inline Value value() const {
            return this->m_value;
        }

        [[nodiscard]] inline Location location() const {
            return this->m_location;
        }

        Token() = default;

    private:
        Type m_type{};
        Value m_value;
        Location m_location{};

    public:
        static std::map<std::string, Token>& operators();
        static std::map<char,             Token>& separators();
        static std::map<std::string, Token>& keywords();
    };

    inline Token makeToken(Token::Type type, const Token::Value& value) {
        return Token(type, value, { 1, 1 });
    }

    namespace tokens {

        namespace Keyword {

            inline Token makeKeyword(const Token::Value& value, const std::string &name) {
                auto token = makeToken(Token::Type::Keyword, value);
                Token::keywords()[name] = token;
                return token;
            }

            const auto If = makeKeyword(Token::Keyword::If, "if");
            const auto Else = makeKeyword(Token::Keyword::Else, "else");
            const auto Return = makeKeyword(Token::Keyword::Return, "return");
            const auto Func = makeKeyword(Token::Keyword::Func, "func");

        }

        namespace Operator {

            constexpr static u8 maxOperatorLength = 1;

            inline Token makeOperator(const Token::Value& value, const std::string &name) {
                auto token = makeToken(Token::Type::Operator, value);
                Token::operators()[name] = token;
                return token;
            }

            const auto Plus = makeOperator(Token::Operator::Plus, "+");
            const auto Minus = makeOperator(Token::Operator::Minus, "-");
            const auto Multiply = makeOperator(Token::Operator::Multiply, "*");
            const auto Divide = makeOperator(Token::Operator::Divide, "/");
            const auto Assign = makeOperator(Token::Operator::Assign, "=");

        }

        namespace Separator {

            inline Token makeSeparator(const Token::Value& value, char name) {
                auto token = makeToken(Token::Type::Separator, value);
                Token::separators()[name] = token;
                return token;
            }

            const auto LeftParenthesis = makeSeparator(Token::Separator::LeftParenthesis, '(');
            const auto RightParenthesis = makeSeparator(Token::Separator::RightParenthesis, ')');
            const auto LeftBrace = makeSeparator(Token::Separator::LeftBrace, '{');
            const auto RightBrace = makeSeparator(Token::Separator::RightBrace, '}');
            const auto LeftBracket = makeSeparator(Token::Separator::LeftBracket, '[');
            const auto RightBracket = makeSeparator(Token::Separator::RightBracket, ']');
            const auto Comma = makeSeparator(Token::Separator::Comma, ',');
            const auto Dot = makeSeparator(Token::Separator::Dot, '.');
            const auto Semicolon = makeSeparator(Token::Separator::Semicolon, ';');

            const auto EndOfProgram = makeToken(Token::Type::Separator, Token::Separator::EndOfProgram);

        }

        namespace Literal {

            inline Token makeIdentifier(const std::string &identifier) {
                return makeToken(Token::Type::Identifier, { Token::Identifier(identifier) });
            }

            inline Token makeString(const std::string &string) {
                return makeToken(Token::Type::String, { string });
            }

            inline Token makeNumeric(const Token::Value& value) {
                return makeToken(Token::Type::Integer, value);
            }

        }


    }

}