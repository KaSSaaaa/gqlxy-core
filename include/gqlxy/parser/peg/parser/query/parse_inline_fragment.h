#pragma once

#include <gqlxy/parser/ast/selection.h>
#include <graphqlservice/internal/SyntaxTree.h>

namespace gqlxy::parser {

InlineFragment ParseInlineFragment(const graphql::peg::ast_node& node);

}