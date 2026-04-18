#include <gqlxy/utils/peg/is_type.h>
#include <gqlxy/parser/peg/parser/query/parse_document.h>
#include <gqlxy/parser/peg/parser/query/parse_fragment_definition.h>
#include <gqlxy/parser/peg/parser/query/parse_operation_definition.h>
#include <gqlxy/utils/ranges.h>
#include <graphqlservice/internal/Grammar.h>

using namespace std;
using namespace graphql;
using namespace gqlxy::utils;

namespace gqlxy::parser {

Document ParseDocument(const string& query) {
    try {
        auto ast = peg::parseString(query);
        if (!ast.root) return {};

        return {
            .operations = to_vector(ast.root->children
                | views::filter(is_type<peg::operation_definition>())
                | views::transform([](const auto& child) {
                    return ParseOperationDefinition(*child);
                })),
            .fragments = to_unordered_map(ast.root->children
                | views::filter(is_type<peg::fragment_definition>())
                | views::transform([](const auto& child) {
                    auto frag = ParseFragmentDefinition(*child);
                    return make_pair(frag.name, frag);
                }))
        };
    } catch (const exception&) {
        return {};
    }
}

}