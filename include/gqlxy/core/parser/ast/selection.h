#pragma once

#include <gqlxy/core/parser/ast/argument.h>
#include <gqlxy/core/parser/ast/directive.h>
#include <gqlxy/core/parser/ast/selection_set.h>
#include <gqlxy/core/utils/optional.h>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace gqlxy::parser {

struct Field {
    std::optional<std::string> alias;
    std::string name;
    std::vector<Argument> arguments;
    std::vector<Directive> directives;
    std::optional<SelectionSet> selectionSet;
};

struct FragmentSpread {
    std::string name;
    std::vector<Directive> directives;
};

struct InlineFragment {
    std::optional<std::string> typeCondition;
    std::vector<Directive> directives;
    std::optional<SelectionSet> selectionSet;
};

struct Selection : std::variant<Field, FragmentSpread, InlineFragment> {};

}

template <>
struct std::formatter<gqlxy::parser::Field> : std::formatter<std::string> {
    auto format(const gqlxy::parser::Field& f, std::format_context& ctx) const {
        return std::formatter<std::string>::format(
            std::format("{}{}{}{}{}", gqlxy::utils::and_then(f.alias, [](const auto& t) {
                return std::format("{}: ", t);
            }),
            f.name,
            !f.arguments.empty() ? std::format("({})", f.arguments) : "",
            !f.directives.empty() ? std::format(" {}", f.directives) : "",
            gqlxy::utils::and_then(f.selectionSet, [](const auto& s) {
                return make_optional(std::vformat(" {}", std::make_format_args(s)));
            }).value_or("")),
        ctx);
    }
};

template <>
struct std::formatter<gqlxy::parser::FragmentSpread> : std::formatter<std::string> {
    auto format(const gqlxy::parser::FragmentSpread& f, std::format_context& ctx) const {
        return std::formatter<std::string>::format(
            std::format("...{}{}", f.name, !f.directives.empty() ? std::format(" {}", f.directives) : ""),
        ctx);
    }
};

template <>
struct std::formatter<gqlxy::parser::InlineFragment> : std::formatter<std::string> {
    auto format(const gqlxy::parser::InlineFragment& f, std::format_context& ctx) const {
        return std::formatter<std::string>::format(
            std::format("...{}{}", gqlxy::utils::and_then(f.typeCondition, [](const auto& t) {
                return make_optional(std::format(" on {}", t));
            }).value_or(""),
            gqlxy::utils::and_then(f.selectionSet, [](const auto& s) {
                return make_optional(std::vformat(" {}", std::make_format_args(s)));
            }).value_or("")
        ), ctx);
    }
};

template <>
struct std::formatter<gqlxy::parser::Selection> : std::formatter<std::string> {
    auto format(const gqlxy::parser::Selection& s, std::format_context& ctx) const {
        return std::formatter<std::string>::format(std::visit([](const auto& v) {
            return std::vformat("{}", std::make_format_args(v));
        }, s), ctx);
    }
};

template <>
struct std::formatter<gqlxy::parser::SelectionSet> : std::formatter<std::string> {
    auto format(const gqlxy::parser::SelectionSet& ss, std::format_context& ctx) const {
        return std::formatter<std::string>::format(
            std::format("{{ {} }}", ss.selections | views::transform([](const auto& s) {
                return std::format("{}", s);
            }) | gqlxy::utils::join_with(" ")
        ), ctx);
    }
};
