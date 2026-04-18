#pragma once

#include <vector>

namespace gqlxy::parser {
struct Selection;

struct SelectionSet {
    std::vector<Selection> selections;
};

}