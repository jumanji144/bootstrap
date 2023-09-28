#include <iostream>
#include "parser/lexer.h"

int main() {
    parser::Lexer lexer;
    auto result = lexer.lex("u32 a = 3;");

    parser::Token token = result.unwrap()[3];
    parser::Token::Literal literal = std::get<parser::Token::Literal>(token.value());

    s64 value = std::get<s64>(literal);

    return 0;
}
