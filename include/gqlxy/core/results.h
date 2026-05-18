#pragma once

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace gqlxy {

struct ErrorLocation {
    int line = 0;
    int column = 0;
};

struct GraphQLError {
    std::string message;
    std::vector<std::string> path = {};
    std::vector<ErrorLocation> locations = {};
    std::optional<nlohmann::json> extensions = std::nullopt;
};

using GraphQLErrors = std::vector<GraphQLError>;

struct GraphQLResponse {
    std::optional<nlohmann::json> data;
    std::optional<GraphQLErrors> errors;
};

nlohmann::json Serialize(const GraphQLResponse& result);

}
