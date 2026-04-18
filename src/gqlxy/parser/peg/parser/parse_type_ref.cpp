#include <gqlxy/parser/peg/parser/parse_type_ref.h>

#include <gqlxy/utils/peg/first_node.h>
#include <gqlxy/utils/peg/is_type.h>
#include <gqlxy/utils/optional.h>
#include <graphqlservice/internal/Grammar.h>

using namespace std;
using namespace graphql;
using namespace gqlxy::utils;

namespace gqlxy::parser {

TypeRef ParseTypeRefFromNode(const peg::ast_node& node) {
    return or_else(and_then(first_node<peg::nonnull_type>(node), [](const auto& nnt) {
        return make_optional(ParseTypeRef(*nnt));
    }), [&node]() {
        return or_else(and_then(first_node<peg::list_type>(node), [](const auto& lt) {
            return make_optional(ParseTypeRef(*lt));
        }), [&node]() {
            return and_then(first_node<peg::named_type>(node), [](const auto& nt) {
                return make_optional(ParseTypeRef(*nt));
            });
        });
    }).value();
}

TypeRef ParseTypeRef(const peg::ast_node& node) {
    if (node.is_type<peg::named_type>()) return TypeRef::Named(node.string());
    auto it = ranges::find_if(node.children, [](const auto& child) {
        return is_type<peg::nonnull_type, peg::list_type, peg::named_type>(*child);
    });

    if (it == node.children.end()) return TypeRef::Named("Unknown");
    auto innerType = ParseTypeRef(*it->get());
    return node.is_type<peg::nonnull_type>() ? TypeRef::NonNull(innerType) : TypeRef::List(innerType);
}

}
