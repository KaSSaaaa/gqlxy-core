#pragma once

#include <graphqlservice/internal/SyntaxTree.h>
#include <gqlxy/parser/ast/variable_definition.h>

namespace gqlxy::parser {

VariableDefinition ParseVariableDefinition(const graphql::peg::ast_node& node);

}