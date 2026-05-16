#pragma once

#include "fragment_definition.h"
#include "operation_definition.h"

#include <format>
#include <string>
#include <unordered_map>
#include <vector>

namespace gqlxy::parser {

struct Document {
    std::vector<OperationDefinition> operations;
    std::unordered_map<std::string, FragmentDefinition> fragments;
};

}

template <>
struct std::formatter<gqlxy::parser::Document> : std::formatter<std::string> {
    auto format(const gqlxy::parser::Document& d, std::format_context& ctx) const {
        auto result = d.operations
            | views::transform([](const auto& o) { return std::format("{}", o); })
            | gqlxy::utils::join_with("\n");
        if (!d.operations.empty() && !d.fragments.empty()) result += '\n';
        if (!d.fragments.empty())
            result += std::format("{}", d.fragments
                | views::values
                | views::transform([](const auto& f) { return std::format("{}", f); })
                | gqlxy::utils::join_with("\n"));
        return std::formatter<std::string>::format(result, ctx);
    }
};
