#include <gqlxy/utils/peg/first_node.h>
#include <gqlxy/utils/peg/is_type.h>
#include <gqlxy/parser/peg/parser/query/parse_arguments.h>
#include <gqlxy/parser/peg/parser/query/parse_directives.h>
#include <gqlxy/utils/optional.h>
#include <gqlxy/utils/ranges.h>
#include <graphqlservice/internal/Grammar.h>

using namespace std;
using namespace graphql;
using namespace gqlxy::utils;

namespace gqlxy::parser {

vector<Directive> ParseDirectives(const peg::ast_node& node) {
    return and_then(first_node<peg::directives>(node), [](const auto* directives) {
        return make_optional(to_vector(directives->children
            | views::filter(is_type<peg::directive>())
            | views::transform([](const auto& child) {
                return Directive{
                    .name = find_node<peg::directive_name>(*child).value()->string(),
                    .args = ParseArguments(*child),
                };
            })));
    }).value_or(vector<Directive>{});
}

}
