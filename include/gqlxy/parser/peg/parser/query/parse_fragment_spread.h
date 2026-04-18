#pragma once
#include <gqlxy/parser/ast/selection.h>
#include <graphqlservice/internal/SyntaxTree.h>

namespace gqlxy::parser {

FragmentSpread ParseFragmentSpread(const graphql::peg::ast_node& node);

}