#pragma once

#include <stdexcept>

namespace gqlxy::utils {

template<typename TException = std::runtime_error>
void expect(bool condition, const std::string& message) {
    if (!condition) throw TException(message);
}

}