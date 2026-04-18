#include <gqlxy/parser/ast/selection_set.h>
#include <gqlxy/utils/peg/first_node.h>
#include <gqlxy/parser/peg/parser/query/parse_fragment_definition.h>
#include <gqlxy/parser/peg/parser/query/parse_selection_set.h>
#include <gqlxy/utils/optional.h>
#include <graphqlservice/internal/Grammar.h>

using namespace std;
using namespace graphql;
using namespace gqlxy::utils;

namespace gqlxy::parser {

FragmentDefinition ParseFragmentDefinition(const peg::ast_node& node) {
    return FragmentDefinition {
        .name = and_then(first_node<peg::fragment_name>(node), [](const auto* n) {
            return n->string();
        }),
        .typeCondition = and_then(first_node<peg::type_condition>(node), [](const auto* tc) {
            return and_then(first_node<peg::named_type>(*tc), [](const auto* n) {
                return make_optional(n->string());
            });
        }).value_or(""),
        .selectionSet = and_then(first_node<peg::selection_set>(node), [](const auto* ss) {
            return make_optional(ParseSelectionSet(*ss));
        }).value_or(SelectionSet{}),
    };
}

}