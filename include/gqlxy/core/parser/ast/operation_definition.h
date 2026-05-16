#pragma once

#include "variable_definition.h"
#include <better-enums/enum.h>
#include <gqlxy/core/parser/ast/selection.h>
#include <optional>
#include <string>
#include <vector>

namespace gqlxy::parser {

BETTER_ENUM(OperationType, int, QUERY, MUTATION, SUBSCRIPTION);

struct OperationDefinition {
    OperationType type = OperationType::QUERY;
    std::optional<std::string> name;
    std::vector<VariableDefinition> variableDefinitions;
    SelectionSet selectionSet;
};

}

template <>
struct std::formatter<gqlxy::parser::OperationDefinition> : std::formatter<std::string> {
    auto format(const gqlxy::parser::OperationDefinition& o, std::format_context& ctx) const {
        if (o.type._value == gqlxy::parser::OperationType::QUERY && !o.name.has_value() && o.variableDefinitions.empty())
            return std::formatter<std::string>::format(std::format("{}", o.selectionSet), ctx);

        return std::formatter<std::string>::format(
            std::format("{} {}{} {}",
                gqlxy::utils::to_string(std::string(o.type._to_string()) | std::views::transform(::tolower)),
                o.name.value_or(""),
                !o.variableDefinitions.empty() ? std::format("({})", o.variableDefinitions) : "",
                o.selectionSet),
            ctx);
    }
};
