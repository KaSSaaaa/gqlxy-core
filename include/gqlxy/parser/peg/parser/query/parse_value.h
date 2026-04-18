#pragma once

#include <graphqlservice/internal/SyntaxTree.h>

namespace gqlxy::parser {

std::optional<std::string> ParseValue(const graphql::peg::ast_node& node);

}
