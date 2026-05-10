#include <gqlxy/core/parser/peg/parser/query/parse_selection_set.h>

#include <gqlxy/core/utils/peg/is_type.h>
#include <gqlxy/core/parser/peg/parser/query/parse_selection.h>
#include <gqlxy/core/utils/ranges.h>
#include <graphqlservice/internal/Grammar.h>
#include <graphqlservice/internal/SyntaxTree.h>

using namespace std;
using namespace graphql;
using namespace gqlxy::utils;

namespace gqlxy::parser {

SelectionSet ParseSelectionSet(const peg::ast_node& node) {
    return SelectionSet {
        .selections = to_vector(node.children
            | views::filter(is_type<peg::field, peg::fragment_spread, peg::inline_fragment>())
            | views::transform([](const auto& child) { return ParseSelection(*child); }))
    };
}

}