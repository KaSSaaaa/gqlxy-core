#include <gqlxy/core/parser/ast/selection.h>
#include <gqlxy/core/parser/peg/parser/query/parse_document.h>
#include <gqlxy/core/parser/peg/parser/query/parse_operations.h>

using namespace std;

namespace gqlxy::parser {

vector<OperationDefinition> ParseOperations(const string& query) {
    return ParseDocument(query).operations;
}

}

