#pragma once

namespace gqlxy::utils {

template <typename ... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

}