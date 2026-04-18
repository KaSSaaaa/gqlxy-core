#pragma once

#include <gqlxy/parser/introspection/types/type_ref.h>

#include <optional>
#include <string>

namespace gqlxy::parser {

struct VariableDefinition {
    std::string name;
    TypeRef type;
    std::optional<std::string> defaultValue;
};

}
