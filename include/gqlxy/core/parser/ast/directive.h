#pragma once

#include "argument.h"
#include <format>
#include <string>
#include <vector>

namespace gqlxy::parser {

struct Directive {
    std::string name;
    std::vector<Argument> args;
};

}

template <>
struct std::formatter<gqlxy::parser::Directive> : std::formatter<std::string> {
    auto format(const gqlxy::parser::Directive& d, std::format_context& ctx) const {
        if (d.args.empty()) return std::formatter<std::string>::format(std::format("@{}", d.name), ctx);
        return std::formatter<std::string>::format(std::format("@{}({})", d.name, d.args), ctx);
    }
};

template <>
struct std::formatter<std::vector<gqlxy::parser::Directive>> : std::formatter<std::string> {
    auto format(const std::vector<gqlxy::parser::Directive>& directives, std::format_context& ctx) const {
        return std::formatter<std::string>::format(directives | views::transform([](const auto& d) {
            return std::format("{}", d);
        }) | gqlxy::utils::join_with(" "), ctx);
    }
};
