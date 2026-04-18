#pragma once

#include <unordered_map>
#include <string>

namespace gqlxy::parser {
struct FragmentDefinition;

using Fragments = std::unordered_map<std::string, FragmentDefinition>;

}