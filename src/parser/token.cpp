#include <parser/token.hpp>

using namespace parser;

std::map<std::string, Token>& Token::operators() {
    static std::map<std::string, Token> s_operators;

    return s_operators;
}

std::map<std::string, Token>& Token::keywords() {
    static std::map<std::string, Token> s_keywords;

    return s_keywords;
}

std::map<char, Token>& Token::separators() {
    static std::map<char, Token> s_separators;

    return s_separators;
}