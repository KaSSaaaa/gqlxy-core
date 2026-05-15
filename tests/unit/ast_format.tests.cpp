#include <algorithm>
#include <format>
#include <functional>
#include <gqlxy/core/parser/ast/argument.h>
#include <gqlxy/core/parser/ast/directive.h>
#include <gqlxy/core/parser/ast/document.h>
#include <gqlxy/core/parser/ast/fragment_definition.h>
#include <gqlxy/core/parser/ast/operation_definition.h>
#include <gqlxy/core/parser/ast/selection.h>
#include <gqlxy/core/parser/ast/variable_definition.h>
#include <gtest/gtest.h>

using namespace std;
using namespace gqlxy::parser;

struct AstFormatCase {
    function<string()> format;
    string expected;
};

class AstFormat : public testing::TestWithParam<AstFormatCase> {};

TEST_P(AstFormat, FormatsCorrectly) {
    EXPECT_EQ(GetParam().format(), GetParam().expected);
}

INSTANTIATE_TEST_SUITE_P(
    Argument, AstFormat,
    testing::Values(
        AstFormatCase {
            .format = [] { return format("{}", Argument {.name = "id", .value = "42"}); },
            .expected = "id: 42"
        },
        AstFormatCase {
            .format = [] { return format("{}", Argument {.name = "id", .value = "$id"}); },
            .expected = "id: $id"
        },
        AstFormatCase {
            .format = [] { return format("{}", Argument {.name = "active", .value = "true"}); },
            .expected = "active: true"
        }));

INSTANTIATE_TEST_SUITE_P(
    Directive, AstFormat,
    testing::Values(
        AstFormatCase {
            .format = [] { return format("{}", Directive {.name = "deprecated"}); },
            .expected = "@deprecated"
        },
        AstFormatCase {
            .format = [] {
                return format("{}", Directive {
                    .name = "skip", .args = {
                        Argument {.name = "if", .value = "true"}
                    }
                });
            },
            .expected = "@skip(if: true)"
        },
        AstFormatCase {
            .format = [] {
                return format("{}", Directive {
                    .name = "foo",
                    .args = {
                        Argument {.name = "a", .value = "1"},
                        Argument {.name = "b", .value = "2"}
                    }});
                },
            .expected = "@foo(a: 1, b: 2)"
        }));

INSTANTIATE_TEST_SUITE_P(
    VariableDefinition, AstFormat,
    testing::Values(
        AstFormatCase {
            .format = [] {
                return format("{}", VariableDefinition {
                    .name = "id", .type = TypeRef::NonNull(TypeRef::Named("ID"))
                });
            },
            .expected = "$id: ID!"
        },
        AstFormatCase {
            .format = [] {
                return format("{}", VariableDefinition {
                    .name = "limit", .type = TypeRef::Named("Int")
                });
            },
            .expected = "$limit: Int"
        },
        AstFormatCase {
            .format = [] {
                return format("{}", VariableDefinition {
                    .name = "limit", .type = TypeRef::Named("Int"), .defaultValue = "10"
                });
            },
            .expected = "$limit: Int = 10"
        }));

INSTANTIATE_TEST_SUITE_P(
    Field, AstFormat,
    testing::Values(
        AstFormatCase {
            .format = [] { return format("{}", Field {.name = "hero"}); },
            .expected = "hero"
        },
        AstFormatCase {
            .format = [] { return format("{}", Field {.alias = "myHero", .name = "hero"}); },
            .expected = "myHero: hero"
        },
        AstFormatCase {
            .format = [] {
                return format("{}", Field {
                    .name = "hero", .arguments = {
                        Argument {.name = "id", .value = R"("1")"}
                    }
                });
            },
            .expected = R"(hero(id: "1"))"
        },
        AstFormatCase {
            .format = [] {
                return format("{}", Field {
                    .name = "hero",
                    .directives = {
                        Directive {.name = "skip", .args = {
                            Argument {.name = "if", .value = "$s"}
                        }}
                    }
                });
            },
            .expected = "hero @skip(if: $s)"
        },
        AstFormatCase {
            .format = [] {
                return format("{}", Field {
                    .name = "hero",
                    .selectionSet = SelectionSet {
                        .selections = {
                            Selection {Field {.name = "id"}}
                        }
                    }
                });
            },
            .expected = "hero { id }"
        }));

INSTANTIATE_TEST_SUITE_P(
    FragmentSpread, AstFormat,
    testing::Values(
        AstFormatCase {
            .format = [] { return format("{}", FragmentSpread {.name = "HeroFields"}); },
            .expected = "...HeroFields"},
        AstFormatCase {
            .format = [] {
                return format("{}", FragmentSpread {
                    .name = "HeroFields",
                    .directives = {
                        Directive {.name = "skip", .args = {
                            Argument {.name = "if", .value = "true"}
                        }}
                    }
                });
            },
            .expected = "...HeroFields @skip(if: true)"
        }));

INSTANTIATE_TEST_SUITE_P(
    InlineFragment, AstFormat,
    testing::Values(
        AstFormatCase {
            .format = [] {
                return format("{}", InlineFragment {
                    .typeCondition = "Droid",
                    .selectionSet = SelectionSet {
                        .selections = {
                            Selection {Field {.name = "primaryFunction"}}
                        }
                    }
                });
            },
            .expected = "... on Droid { primaryFunction }"
        },
        AstFormatCase {
            .format = [] {
                return format("{}", InlineFragment {
                    .selectionSet = SelectionSet {
                        .selections = {
                            Selection {Field {.name = "id"}}
                        }
                    }
                });
                },
            .expected = "... { id }"
        }));

INSTANTIATE_TEST_SUITE_P(
    OperationDefinition, AstFormat,
    testing::Values(
        AstFormatCase {
            .format = [] {
                return format("{}", Document {
                    .operations = {
                        OperationDefinition {
                            .selectionSet = {
                                .selections = {
                                    Selection {Field {.name = "hero"}}
                                }
                            }
                        }
                    }
                });
            },
            .expected = "{ hero }"
        },
        AstFormatCase {
            .format = [] {
                return format("{}", Document {
                    .operations = {
                        OperationDefinition {
                            .type = OperationType::QUERY,
                            .name = "GetHero",
                            .selectionSet = {
                                .selections = {
                                    Selection {Field {.name = "hero"}}
                                }
                            }
                        }
                    }
                });
            },
            .expected = "query GetHero { hero }"},
        AstFormatCase {
            .format = [] {
                return format("{}", Document {
                    .operations = {
                        OperationDefinition {
                            .type = OperationType::MUTATION,
                            .name = "CreateUser",
                            .selectionSet = {
                                .selections = {
                                    Selection {Field {.name = "createUser"}}
                                }
                            }
                        }
                    }
                });
            },
            .expected = "mutation CreateUser { createUser }"},
        AstFormatCase {
            .format = [] {
                return format("{}", Document {
                    .operations = {
                        OperationDefinition {
                            .type = OperationType::SUBSCRIPTION,
                            .name = "OnMessage",
                            .selectionSet = {
                                .selections = {
                                    Selection {Field {.name = "messageAdded"}}
                                }
                            }
                        }
                    }
                });
            },
            .expected = "subscription OnMessage { messageAdded }"},
        AstFormatCase {
            .format = [] {
                return format("{}", Document {
                    .operations = {
                        OperationDefinition {
                            .type = OperationType::QUERY,
                            .name = "GetHero",
                            .variableDefinitions = {
                                VariableDefinition {
                                    .name = "id",
                                    .type = TypeRef::NonNull(TypeRef::Named("ID"))
                                }
                            },
                            .selectionSet = {
                                .selections = {
                                    Selection {Field {.name = "hero"}}
                                }
                            }
                        }
                    }
                });
            },
            .expected = "query GetHero($id: ID!) { hero }"
        }));

INSTANTIATE_TEST_SUITE_P(
    FragmentDefinition, AstFormat,
    testing::Values(
        AstFormatCase {
            .format = [] {
                return format("{}", Document {
                    .fragments = {
                        {"HeroFields", FragmentDefinition {
                            .name = "HeroFields",
                            .typeCondition = "Hero",
                            .selectionSet = SelectionSet {
                            .selections = {
                                Selection {Field {.name = "id"}},
                                Selection {Field {.name = "name"}},
                            }}
                        }}
                    }
                });
            },
            .expected = "fragment HeroFields on Hero { id name }"
        },
        AstFormatCase {
            .format = [] {
                return format("{}", Document {
                    .fragments = {
                        {"UserFields", FragmentDefinition {
                            .name = "UserFields",
                            .typeCondition = "User",
                            .selectionSet = SelectionSet {
                            .selections = {
                                Selection {Field {.name = "name"}},
                                Selection {Field {.name = "email"}},
                            }}
                        }}
                    }
                });
            },
            .expected = "fragment UserFields on User { name email }"}));

INSTANTIATE_TEST_SUITE_P(
    Document, AstFormat,
    testing::Values(
        AstFormatCase {
            .format = [] {
                return format("{}", Document {
                    .operations = {
                        OperationDefinition {
                            .selectionSet = {
                                .selections = {
                                    Selection {Field {.name = "hero"}}
                                }
                            }
                        }
                    }
                });
            },
            .expected = "{ hero }"
        },
        AstFormatCase {
            .format = [] {
                return format("{}", Document {
                    .operations = {
                        OperationDefinition {
                            .name = "GetHero",
                            .selectionSet = {
                                .selections = {
                                    Selection {Field {.name = "hero"}}
                                }
                            }
                        },
                        OperationDefinition {
                            .type = OperationType::MUTATION,
                            .name = "CreateUser",
                            .selectionSet = {
                                .selections = {
                                    Selection {Field {.name = "createUser"}}
                                }
                            }
                        }
                    }
                });
            },
            .expected = "query GetHero { hero }\nmutation CreateUser { createUser }"
        },
        AstFormatCase {
            .format =
                [] {
                return format("{}", Document {
                    .operations = {
                        OperationDefinition {
                            .selectionSet = {
                                .selections = {
                                    Selection {Field{
                                        .name = "user",
                                        .selectionSet = SelectionSet {
                                            .selections = {
                                                Selection{FragmentSpread{.name = "UserFields"}}
                                            }
                                        }
                                    }}
                                }
                            }
                        }
                    },
                    .fragments = {
                        {"UserFields", FragmentDefinition {
                            .name = "UserFields",
                            .typeCondition = "User",
                            .selectionSet = SelectionSet {
                                .selections = {
                                    Selection {Field {.name = "name"}},
                                }
                            }
                        }}
                    }
                });
            },
            .expected = "{ user { ...UserFields } }\nfragment UserFields on User { name }"
        }));
