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

std::string FormatSelection(const Selection&);
std::string FormatSelectionSet(const SelectionSet&);

}

template <>
struct std::formatter<gqlxy::parser::SelectionSet> : std::formatter<std::string> {
    auto format(const gqlxy::parser::SelectionSet& ss, std::format_context& ctx) const {
        return std::formatter<std::string>::format(gqlxy::parser::FormatSelectionSet(ss), ctx);
    }
};

template <>
struct std::formatter<gqlxy::parser::Field> : std::formatter<std::string> {
    auto format(const gqlxy::parser::Field& f, std::format_context& ctx) const {
        return std::formatter<std::string>::format(
            std::format("{}{}{}{}{}", gqlxy::utils::and_then(f.alias, [](const auto& t) {
                return std::make_optional(std::format("{}: ", t));
            }).value_or(""),
            f.name,
            !f.arguments.empty() ? std::format("({})", f.arguments) : "",
            !f.directives.empty() ? std::format(" {}", f.directives) : "",
            gqlxy::utils::and_then(f.selectionSet, [](const auto& s) {
                return std::make_optional(" " + gqlxy::parser::FormatSelectionSet(s));
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
                return std::make_optional(std::format(" on {}", t));
            }).value_or(""),
            gqlxy::utils::and_then(f.selectionSet, [](const auto& s) {
                return std::make_optional(" " + gqlxy::parser::FormatSelectionSet(s));
            }).value_or("")
        ), ctx);
    }
};

template <>
struct std::formatter<gqlxy::parser::Selection> : std::formatter<std::string> {
    auto format(const gqlxy::parser::Selection& s, std::format_context& ctx) const {
        return std::formatter<std::string>::format(gqlxy::parser::FormatSelection(s), ctx);
    }
};

namespace gqlxy::parser {

inline std::string FormatSelection(const Selection& s) {
    return std::visit([](const auto& v) {
        return std::vformat("{}", std::make_format_args(v));
    }, s);
}

inline std::string FormatSelectionSet(const SelectionSet& ss) {
    return std::format("{{ {} }}", ss.selections
        | std::views::transform([](const auto& s) { return FormatSelection(s); })
        | utils::join_with(" "));
}

}
