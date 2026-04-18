#include <gqlxy/parser/ast/selection.h>
#include <gqlxy/parser/peg/parser/query/parse_field.h>
#include <gqlxy/parser/peg/parser/query/parse_fragment_spread.h>
#include <gqlxy/parser/peg/parser/query/parse_inline_fragment.h>
#include <gqlxy/parser/peg/parser/query/parse_selection.h>
#include <graphqlservice/internal/Grammar.h>

using namespace std;
using namespace graphql;

namespace gqlxy::parser {

Selection ParseSelection(const peg::ast_node& node) {
    if (node.is_type<peg::field>())
        return { ParseSelectionField(node) };
    if (node.is_type<peg::fragment_spread>())
        return { ParseFragmentSpread(node) };
    return { ParseInlineFragment(node) };
}

}