#pragma once

#include <gqlxy/parser/ast/operation_definition.h>
#include <string>

namespace gqlxy::parser {

std::vector<OperationDefinition> ParseOperations(const std::string& query);

}
