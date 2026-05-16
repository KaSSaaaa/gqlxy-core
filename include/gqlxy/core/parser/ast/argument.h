#pragma once

#include <gqlxy/core/utils/ranges.h>
#include <format>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>

namespace gqlxy::parser {

struct Argument {
    std::string name;
    std::string value;

    bool IsVariable() const;
    std::string Reference() const;
    static std::string Reference(const std::string& name);
    std::optional<nlohmann::json> TryValue(const nlohmann::json& variables) const;
    nlohmann::json Value(const nlohmann::json& variables) const;
};

}

template <>
struct std::formatter<gqlxy::parser::Argument> : std::formatter<std::string> {
    auto format(const gqlxy::parser::Argument& a, std::format_context& ctx) const {
        return std::formatter<std::string>::format(std::format("{}: {}", a.name, a.value), ctx);
    }
};

template <>
struct std::formatter<std::vector<gqlxy::parser::Argument>> : std::formatter<std::string> {
    auto format(const std::vector<gqlxy::parser::Argument>& a, std::format_context& ctx) const {
        return std::formatter<std::string>::format(a | views::transform([&](const auto& arg) {
            return std::format("{}", arg);
        }) | gqlxy::utils::join_with(", "), ctx);
    }
};
