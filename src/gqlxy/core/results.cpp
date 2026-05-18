#include <gqlxy/core/results.h>
#include <gqlxy/core/utils/ranges.h>

using namespace std;
using namespace nlohmann;
using namespace gqlxy::utils;

namespace gqlxy {

json Serialize(const GraphQLResponse& result) {
    json r;
    if (result.data) r["data"] = *result.data;
    if (result.errors) {
        auto err = json::array();
        for (const auto& e : *result.errors) {
            json entry = {{"message", e.message}};
            if (!e.path.empty()) entry["path"] = e.path;
            if (!e.locations.empty())
                entry["locations"] = to_vector(e.locations | views::transform([](const auto& loc) -> json {
                    return {{"line", loc.line}, {"column", loc.column}};
                }));
            if (e.extensions.has_value()) entry["extensions"] = *e.extensions;
            err.emplace_back(std::move(entry));
        }
        r["errors"] = err;
    }
    return r;
}

}
