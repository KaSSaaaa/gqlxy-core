#pragma once

#include <format>
#include <gqlxy/core/parser/introspection/types/type_ref.h>
#include <gqlxy/core/utils/ranges.h>
#include <optional>
#include <string>

namespace gqlxy::parser {

struct VariableDefinition {
    std::string name;
    TypeRef type;
    std::optional<std::string> defaultValue;
};

}

template <>
struct std::formatter<gqlxy::parser::VariableDefinition> : std::formatter<std::string> {
    auto format(const gqlxy::parser::VariableDefinition& v, std::format_context& ctx) const {
        return std::formatter<std::string>::format(v.defaultValue.has_value()
            ? std::format("${}: {} = {}", v.name, v.type, *v.defaultValue)
            : std::format("${}: {}", v.name, v.type), ctx);
    }
};

template <>
struct std::formatter<std::vector<gqlxy::parser::VariableDefinition>> : std::formatter<std::string> {
    auto format(const std::vector<gqlxy::parser::VariableDefinition>& v, std::format_context& ctx) const {
        return std::formatter<std::string>::format(v | views::transform([&](const auto& arg) {
            return std::format("{}", arg);
        }) | gqlxy::utils::join_with(", "), ctx);
    }
};
