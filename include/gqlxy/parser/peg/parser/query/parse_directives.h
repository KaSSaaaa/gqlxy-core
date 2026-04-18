#pragma once

#include <gqlxy/parser/ast/directive.h>
#include <graphqlservice/internal/SyntaxTree.h>
#include <vector>

namespace gqlxy::parser {

std::vector<Directive> ParseDirectives(const graphql::peg::ast_node& node);

}
