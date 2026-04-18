#include <gqlxy/utils/peg/first_node.h>
#include <gqlxy/utils/peg/is_type.h>
#include <gqlxy/parser/peg/parser/query/parse_arguments.h>
#include <gqlxy/parser/peg/parser/query/parse_value.h>
#include <gqlxy/utils/optional.h>
#include <gqlxy/utils/ranges.h>
#include <graphqlservice/internal/Grammar.h>

using namespace std;
using namespace graphql;
using namespace gqlxy::utils;

namespace gqlxy::parser {

vector<Argument> ParseArguments(const peg::ast_node& node) {
    return and_then(first_node<peg::arguments>(node), [](const auto* argsNode) {
        return make_optional(to_vector(argsNode->children
            | views::filter(is_type<peg::argument>())
            | views::transform([](const auto& child) {
                return Argument {
                    .name = and_then(first_node<peg::argument_name>(*child), [](const auto* n) { return n->string(); }),
                    .value = ParseValue(*child).value_or(""),
                };
            })));
    }).value_or(vector<Argument>{});
}

}