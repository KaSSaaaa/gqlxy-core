#pragma once

#include <gqlxy/parser/ast/document.h>

namespace gqlxy::parser {

Document ParseDocument(const std::string& query);

}