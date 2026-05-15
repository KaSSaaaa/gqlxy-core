#pragma once

#include <format>
#include <gqlxy/core/parser/ast/selection.h>
#include <string>

namespace gqlxy::parser {

struct FragmentDefinition {
    std::string name;
    std::string typeCondition;
    SelectionSet selectionSet;
};

}

template <>
struct std::formatter<gqlxy::parser::FragmentDefinition> : std::formatter<std::string> {
    auto format(const gqlxy::parser::FragmentDefinition& f, std::format_context& ctx) const {
        return std::formatter<std::string>::format(
            std::format("fragment {} on {} {}", f.name, f.typeCondition, f.selectionSet), ctx);
    }
};
