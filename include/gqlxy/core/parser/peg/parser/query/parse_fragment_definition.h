#pragma once

#include <gqlxy/core/parser/ast/fragment_definition.h>
#include <graphqlservice/internal/SyntaxTree.h>

namespace gqlxy::parser {

FragmentDefinition ParseFragmentDefinition(const graphql::peg::ast_node& node);

}
