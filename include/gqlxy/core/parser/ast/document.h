#pragma once

#include "fragment_definition.h"
#include "operation_definition.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace gqlxy::parser {

struct Document {
    std::vector<OperationDefinition> operations;
    std::unordered_map<std::string, FragmentDefinition> fragments;
};

}
