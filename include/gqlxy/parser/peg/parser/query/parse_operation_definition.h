#pragma once

#include <gqlxy/parser/ast/operation_definition.h>
#include <graphqlservice/internal/SyntaxTree.h>

namespace gqlxy::parser {

OperationDefinition ParseOperationDefinition(const graphql::peg::ast_node& node);

}