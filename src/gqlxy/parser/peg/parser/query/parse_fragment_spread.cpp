#include <gqlxy/parser/peg/parser/query/parse_directives.h>
#include <gqlxy/parser/peg/parser/query/parse_fragment_spread.h>
#include <gqlxy/utils/optional.h>
#include <gqlxy/utils/peg/first_node.h>
#include <graphqlservice/internal/Grammar.h>

using namespace std;
using namespace graphql;
using namespace gqlxy::utils;

namespace gqlxy::parser {

FragmentSpread ParseFragmentSpread(const peg::ast_node& node) {
    return FragmentSpread {
        .name = and_then(first_node<peg::fragment_name>(node), [](const auto* n) { return n->string(); }),
        .directives = ParseDirectives(node),
    };
}

}