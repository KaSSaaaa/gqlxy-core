#pragma once

#include <gqlxy/parser/ast/selection_set.h>
#include <graphqlservice/internal/SyntaxTree.h>

namespace gqlxy::parser {

SelectionSet ParseSelectionSet(const graphql::peg::ast_node& node);

}