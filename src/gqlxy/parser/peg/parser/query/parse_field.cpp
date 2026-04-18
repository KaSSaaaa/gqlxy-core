#include <gqlxy/parser/peg/parser/query/parse_arguments.h>
#include <gqlxy/parser/peg/parser/query/parse_directives.h>
#include <gqlxy/parser/peg/parser/query/parse_field.h>
#include <gqlxy/parser/peg/parser/query/parse_selection_set.h>
#include <gqlxy/utils/optional.h>
#include <gqlxy/utils/peg/first_node.h>
#include <graphqlservice/internal/Grammar.h>

using namespace std;
using namespace graphql;
using namespace gqlxy::utils;

namespace gqlxy::parser {

Field ParseSelectionField(const peg::ast_node& node) {
    return Field{
        .alias = and_then(first_node<peg::alias_name>(node), [](const auto* n) {
            return make_optional(n->string());
        }),
        .name = and_then(first_node<peg::field_name>(node), [](const auto* n) {
            return n->string();
        }),
        .arguments = ParseArguments(node),
        .directives = ParseDirectives(node),
        .selectionSet = and_then(first_node<peg::selection_set>(node), [](const auto* n) {
            return make_optional(ParseSelectionSet(*n));
        }),
   };
}

}