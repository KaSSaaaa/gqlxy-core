#pragma once

#include <gqlxy/core/parser/ast/selection.h>
#include <graphqlservice/internal/SyntaxTree.h>

namespace gqlxy::parser {

SelectionSet ParseSelectionSet(const graphql::peg::ast_node& node);

}