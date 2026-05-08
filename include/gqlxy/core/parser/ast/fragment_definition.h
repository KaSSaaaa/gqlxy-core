#pragma once

#include <gqlxy/core/parser/ast/selection.h>

#include <string>

namespace gqlxy::parser {

struct FragmentDefinition {
    std::string name;
    std::string typeCondition;
    SelectionSet selectionSet;
};

}
