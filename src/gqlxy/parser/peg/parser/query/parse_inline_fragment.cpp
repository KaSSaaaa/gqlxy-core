#include <gqlxy/parser/peg/parser/query/parse_directives.h>
#include <gqlxy/parser/peg/parser/query/parse_inline_fragment.h>
#include <gqlxy/parser/peg/parser/query/parse_selection_set.h>
#include <gqlxy/utils/optional.h>
#include <gqlxy/utils/peg/first_node.h>
#include <graphqlservice/internal/Grammar.h>

using namespace std;
using namespace graphql;
using namespace gqlxy::utils;

namespace gqlxy::parser {

InlineFragment ParseInlineFragment(const peg::ast_node& node)  {
    return InlineFragment {
        .typeCondition = and_then(first_node<peg::type_condition>(node), [](const auto* tc) {
            return and_then(first_node<peg::named_type>(*tc), [](const auto* n) {
                return make_optional(n->string());
            });
        }),
        .directives = ParseDirectives(node),
        .selectionSet = and_then(first_node<peg::selection_set>(node), [](const auto* n) {
            return make_optional(ParseSelectionSet(*n));
        }),
    };
}

}