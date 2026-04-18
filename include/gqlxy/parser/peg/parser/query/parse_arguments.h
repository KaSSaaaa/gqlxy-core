#pragma once

#include <gqlxy/parser/ast/argument.h>
#include <graphqlservice/internal/SyntaxTree.h>
#include <vector>

namespace gqlxy::parser {

std::vector<Argument> ParseArguments(const graphql::peg::ast_node& node);

}