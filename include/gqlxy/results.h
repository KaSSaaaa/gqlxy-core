#pragma once

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace gqlxy {

struct ErrorLocation {
    int line;
    int column;
};

struct GraphQLError {
    std::string message;
    std::vector<std::string> path;
    std::vector<ErrorLocation> locations;
};

using GraphQLErrors = std::vector<GraphQLError>;

struct GraphQLResponse {
    std::optional<nlohmann::json> data;
    std::optional<GraphQLErrors> errors;
};

nlohmann::json Serialize(const GraphQLResponse& result);

}
