#pragma once

#include "selection_set.h"

#include <gqlxy/parser/ast/argument.h>
#include <gqlxy/parser/ast/directive.h>
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
