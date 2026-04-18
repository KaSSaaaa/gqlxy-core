#pragma once

#include <gqlxy/parser/introspection/types/type_ref.h>

namespace graphql::peg {
class ast_node;
}

namespace gqlxy::parser {

TypeRef ParseTypeRefFromNode(const graphql::peg::ast_node& node);
TypeRef ParseTypeRef(const graphql::peg::ast_node& node);


}
