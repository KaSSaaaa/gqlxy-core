#pragma once

#include <gqlxy/parser/ast/selection.h>

#include <string>

namespace gqlxy::parser {

struct FragmentDefinition {
    std::string name;
    std::string typeCondition;
    SelectionSet selectionSet;
};

}
