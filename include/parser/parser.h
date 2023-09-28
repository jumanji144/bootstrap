#pragma once
#include <parser/token.hpp>
#include <parser/ast/ast_node.hpp>

#include <common/result.h>

#include <map>
#include <string>
#include <vector>
#include <memory>

namespace parser {

    class Parser {
    public:
        using Iterator = std::vector<Token>::const_iterator;

        Parser() = default;
        ~Parser() = default;

        util::Results<std::vector<std::shared_ptr<ast::AstNode>>> parse(const std::vector<Token>& tokens);

    private:
        // parser functions

        // state
        Iterator m_current;
        Iterator m_resetPoint;

        std::vector<Token> m_tokens;
        std::vector<util::Error> m_errors;
        std::vector<std::shared_ptr<ast::AstNode>> m_ast;

    };

}