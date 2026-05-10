#pragma once

#include <gqlxy/core/parser/ast/document.h>

namespace gqlxy::parser {

Document ParseDocument(const std::string& query);

}