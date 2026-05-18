# gqlxy-core

A C++20 library that serves as the shared foundation between [`gqlxy-server`](../gqlxy-server) and [`gqlxy-client`](../gqlxy-client). It provides the common types, parsers, and utilities that both sides of the GraphQL stack depend on.

## Purpose

`gqlxy-core` is intentionally minimal — it does not implement a server or client on its own. Instead, it defines the shared contract: what a GraphQL document looks like, how responses are structured, and the building blocks (coroutine task, utility helpers) that both consumers rely on.

```
gqlxy-server ──┐
               ├── gqlxy-core
gqlxy-client ──┘
```

## What's inside

### GraphQL Document AST (`gqlxy/parser/ast/`)

C++ structs representing every node in a parsed GraphQL document:

- `Document` — top-level container of operations and fragment definitions
- `OperationDefinition` — query, mutation, or subscription with variable definitions and a selection set
- `SelectionSet` / `Selection` — fields, fragment spreads, and inline fragments
- `Field` — optional alias, arguments, directives, and nested selection set
- `Argument` / `Directive` — with variable resolution support
- `FragmentDefinition` / `VariableDefinition` / `TypeRef`

### GraphQL Parser (`gqlxy/parser/peg/`)

A high-level wrapper around the `cppgraphqlgen` PEG parser:

```cpp
#include <gqlxy/parser/peg/parser/parse_document.h>

gqlxy::parser::Document doc = gqlxy::parser::ParseDocument(queryString);
```

Per-node parse functions are also available for granular use.

### Response Types (`gqlxy/results.h`)

Common response structures shared between server output and client consumption:

```cpp
#include <gqlxy/results.h>

gqlxy::GraphQLResponse response;
nlohmann::json serialized = gqlxy::Serialize(response);
```

Includes `GraphQLResponse`, `GraphQLError`, `GraphQLErrors`, and `ErrorLocation`, with JSON serialization via `nlohmann/json`.

### Coroutine Task (`gqlxy/task.h`)

A C++20 coroutine type for async operations used throughout both `gqlxy-server` and `gqlxy-client`:

```cpp
#include <gqlxy/task.h>

gqlxy::Task<std::string> fetchData() {
    co_return "result";
}
```

### Utilities (`gqlxy/utils/`)

- **`ranges.h`** — `to_vector`, `flat_map`, `find_optional`, `join_with`, `split`, `chunk_by_blank`, and more
- **`optional.h`** — pre-C++23 monadic helpers (`and_then`, `or_else`)
- **`visit.h`** — `overloaded<Ts...>` for `std::visit`
- **`expect.h`** — `expect<TException>(condition, message)`
- **`peg/`** — PEG node traversal helpers (`first_node`, `find_node`, `is_type`)

## Installation

### vcpkg (recommended)

`gqlxy-core` is available as a vcpkg port from a custom registry.

Add the registry and the dependency to your `vcpkg.json`:

```json
{
  "dependencies": ["gqlxy-core"],
  "configuration": {
    ...
    "registries": [
      ...
      {
        "kind": "git",
        "repository": "https://github.com/KaSSaaaa/vcpkg.git",
        "reference": "feature/gqlxy",
        "baseline": "<basline>",
        "packages": ["gqlxy-*"]
      }
    ]
  }
}
```

Then wire it up in CMake:

```cmake
find_package(gqlxy_core CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE gqlxy::core)
```

Configure with the vcpkg toolchain:

```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

## Building

Requires CMake 3.10+ and a C++20-capable compiler. Dependencies are managed via [vcpkg](https://vcpkg.io).

```sh
# Configure (choose a preset: arm64-osx, x64-osx, x64-linux, x64-windows)
cmake --preset arm64-osx-debug

# Build
cmake --build out/build/arm64-osx-debug
```

## Dependencies

| Library | Role |
|---|---|
| [`cppgraphqlgen`](https://github.com/microsoft/cppgraphqlgen) | GraphQL PEG parser |
| [`nlohmann-json`](https://github.com/nlohmann/json) | JSON serialization for responses |
| [`better-enums`](https://github.com/aantron/better-enums) | Reflectable enums |
| [`gtest`](https://github.com/google/googletest) | Unit testing |

Both `cppgraphqlgen` and `nlohmann-json` are linked as `PUBLIC` and are transitively available to consumers of `gqlxy::core`.

## CMake Integration

```cmake
find_package(gqlxy_core REQUIRED)
target_link_libraries(your_target PRIVATE gqlxy::core)
```

## License

See [LICENSE](LICENSE).
