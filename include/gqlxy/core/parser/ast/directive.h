#pragma once

#include "argument.h"

#include <string>
#include <vector>

namespace gqlxy::parser {

struct Directive {
    std::string name;
    std::vector<Argument> args;
};

}
