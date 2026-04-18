#pragma once

#include "variable_definition.h"
#include <better-enums/enum.h>
#include <gqlxy/parser/ast/selection.h>
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
