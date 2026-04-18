#include <gqlxy/parser/ast/operation_definition.h>
#include <gqlxy/parser/ast/selection.h>
#include <gqlxy/parser/peg/parser/query/parse_document.h>
#include <gqlxy/parser/peg/parser/query/parse_operations.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace gqlxy::parser;

static const Field& asField(const Selection& s) {
    return std::get<Field>(s);
}

static const FragmentSpread& asFragmentSpread(const Selection& s) {
    return std::get<FragmentSpread>(s);
}

static const InlineFragment& asInlineFragment(const Selection& s) {
    return std::get<InlineFragment>(s);
}

static std::optional<nlohmann::json> FindArg(const Field& field, const std::string& name,
                                              const nlohmann::json& vars = {}) {
    auto it = std::ranges::find_if(field.arguments, [&](const auto& a) { return a.name == name; });
    if (it == field.arguments.end()) return std::nullopt;
    return it->TryValue(vars);
}

TEST(QueryParser, ParsesShorthandQuery) {
    auto ops = ParseOperations("{ hero }");
    ASSERT_EQ(ops.size(), 1);
    EXPECT_EQ(ops[0].type._value, OperationType::QUERY);
    EXPECT_FALSE(ops[0].name.has_value());
}

TEST(QueryParser, ParsesNamedQuery) {
    auto ops = ParseOperations("query GetHero { hero }");
    ASSERT_EQ(ops.size(), 1);
    EXPECT_EQ(ops[0].type._value, OperationType::QUERY);
    ASSERT_TRUE(ops[0].name.has_value());
    EXPECT_EQ(*ops[0].name, "GetHero");
}

TEST(QueryParser, ParsesMutation) {
    auto ops = ParseOperations("mutation CreateUser { createUser }");
    ASSERT_EQ(ops.size(), 1);
    EXPECT_EQ(ops[0].type._value, OperationType::MUTATION);
    EXPECT_EQ(*ops[0].name, "CreateUser");
}

TEST(QueryParser, ParsesSubscription) {
    auto ops = ParseOperations("subscription OnMessage { messageAdded }");
    ASSERT_EQ(ops.size(), 1);
    EXPECT_EQ(ops[0].type._value, OperationType::SUBSCRIPTION);
    EXPECT_EQ(*ops[0].name, "OnMessage");
}

TEST(QueryParser, ReturnsEmptyOnInvalidQuery) {
    auto ops = ParseOperations("not valid graphql @@@@");
    EXPECT_EQ(ops.size(), 0);
}

TEST(QueryParser, ParsesSingleField) {
    auto ops = ParseOperations("{ hero }");
    ASSERT_EQ(ops[0].selectionSet.selections.size(), 1);
    EXPECT_EQ(asField(ops[0].selectionSet.selections[0]).name, "hero");
}

TEST(QueryParser, ParsesMultipleFields) {
    auto ops = ParseOperations("{ hero villain sidekick }");
    const auto& selections = ops[0].selectionSet.selections;
    ASSERT_EQ(selections.size(), 3);
    EXPECT_EQ(asField(selections[0]).name, "hero");
    EXPECT_EQ(asField(selections[1]).name, "villain");
    EXPECT_EQ(asField(selections[2]).name, "sidekick");
}

TEST(QueryParser, ParsesFieldAlias) {
    auto ops = ParseOperations("{ myHero: hero }");
    const auto& field = asField(ops[0].selectionSet.selections[0]);
    ASSERT_TRUE(field.alias.has_value());
    EXPECT_EQ(*field.alias, "myHero");
    EXPECT_EQ(field.name, "hero");
}

TEST(QueryParser, FieldWithNoAliasHasEmptyOptional) {
    auto ops = ParseOperations("{ hero }");
    const auto& field = asField(ops[0].selectionSet.selections[0]);
    EXPECT_FALSE(field.alias.has_value());
}

TEST(QueryParser, ParsesNestedSelectionSet) {
    auto ops = ParseOperations("{ hero { id name } }");
    const auto& hero = asField(ops[0].selectionSet.selections[0]);
    ASSERT_TRUE(hero.selectionSet.has_value());
    ASSERT_EQ(hero.selectionSet->selections.size(), 2);
    EXPECT_EQ(asField(hero.selectionSet->selections[0]).name, "id");
    EXPECT_EQ(asField(hero.selectionSet->selections[1]).name, "name");
}

TEST(QueryParser, FieldWithNoSubselectionHasNullSelectionSet) {
    auto ops = ParseOperations("{ hero }");
    const auto& field = asField(ops[0].selectionSet.selections[0]);
    EXPECT_FALSE(field.selectionSet.has_value());
}

TEST(QueryParser, ParsesStringArgument) {
    auto ops = ParseOperations(R"({ hero(id: "42") })");
    const auto& field = asField(ops[0].selectionSet.selections[0]);
    ASSERT_EQ(field.arguments.size(), 1);
    EXPECT_EQ(field.arguments[0].name, "id");
    EXPECT_FALSE(field.arguments[0].value.empty());
}

TEST(QueryParser, ParsesIntArgument) {
    auto ops = ParseOperations("{ hero(id: 42) }");
    const auto& field = asField(ops[0].selectionSet.selections[0]);
    ASSERT_EQ(field.arguments.size(), 1);
    EXPECT_EQ(field.arguments[0].name, "id");
    EXPECT_EQ(field.arguments[0].value, "42");
}

TEST(QueryParser, ParsesBoolArgument) {
    auto ops = ParseOperations("{ hero(active: true) }");
    const auto& field = asField(ops[0].selectionSet.selections[0]);
    ASSERT_EQ(field.arguments.size(), 1);
    EXPECT_EQ(field.arguments[0].name, "active");
    EXPECT_EQ(field.arguments[0].value, "true");
}

TEST(QueryParser, ParsesEnumArgument) {
    auto ops = ParseOperations("{ hero(episode: JEDI) }");
    const auto& field = asField(ops[0].selectionSet.selections[0]);
    ASSERT_EQ(field.arguments.size(), 1);
    EXPECT_EQ(field.arguments[0].name, "episode");
    EXPECT_EQ(field.arguments[0].value, "JEDI");
}

TEST(QueryParser, ParsesVariableArgument) {
    auto ops = ParseOperations("query($id: ID!) { hero(id: $id) }");
    const auto& field = asField(ops[0].selectionSet.selections[0]);
    ASSERT_EQ(field.arguments.size(), 1);
    EXPECT_EQ(field.arguments[0].name, "id");
    EXPECT_EQ(field.arguments[0].value, "$id");
}

TEST(QueryParser, ParsesMultipleArguments) {
    auto ops = ParseOperations("{ search(query: \"Luke\", limit: 10) }");
    const auto& field = asField(ops[0].selectionSet.selections[0]);
    ASSERT_EQ(field.arguments.size(), 2);
    EXPECT_EQ(field.arguments[0].name, "query");
    EXPECT_EQ(field.arguments[1].name, "limit");
}

TEST(QueryParser, ParsesSingleVariableDefinition) {
    auto ops = ParseOperations("query($id: ID!) { hero }");
    ASSERT_EQ(ops[0].variableDefinitions.size(), 1);
    EXPECT_EQ(ops[0].variableDefinitions[0].name, "id");
}

TEST(QueryParser, ParsesMultipleVariableDefinitions) {
    auto ops = ParseOperations("query($id: ID! $limit: Int) { hero }");
    ASSERT_EQ(ops[0].variableDefinitions.size(), 2);
    EXPECT_EQ(ops[0].variableDefinitions[0].name, "id");
    EXPECT_EQ(ops[0].variableDefinitions[1].name, "limit");
}

TEST(QueryParser, ParsesVariableTypeNamed) {
    auto ops = ParseOperations("query($id: ID) { hero }");
    ASSERT_EQ(ops[0].variableDefinitions.size(), 1);
    EXPECT_EQ(ops[0].variableDefinitions[0].type.TypeName(), "ID");
}

TEST(QueryParser, ParsesVariableTypeNonNull) {
    auto ops = ParseOperations("query($id: ID!) { hero }");
    const auto& varDef = ops[0].variableDefinitions[0];
    EXPECT_EQ(varDef.type.kind._value, TypeRefKind::NON_NULL);
    EXPECT_EQ(varDef.type.TypeName(), "ID");
}

TEST(QueryParser, ParsesVariableWithDefaultValue) {
    auto ops = ParseOperations("query($limit: Int = 10) { hero }");
    ASSERT_EQ(ops[0].variableDefinitions.size(), 1);
    ASSERT_TRUE(ops[0].variableDefinitions[0].defaultValue.has_value());
    EXPECT_EQ(*ops[0].variableDefinitions[0].defaultValue, "10");
}

TEST(QueryParser, ParsesVariableWithNoDefaultValue) {
    auto ops = ParseOperations("query($id: ID!) { hero }");
    EXPECT_FALSE(ops[0].variableDefinitions[0].defaultValue.has_value());
}

TEST(QueryParser, ParsesFragmentSpread) {
    auto ops = ParseOperations("{ hero { ...HeroFields } }");
    const auto& hero = asField(ops[0].selectionSet.selections[0]);
    ASSERT_EQ(hero.selectionSet->selections.size(), 1);
    const auto& spread = asFragmentSpread(hero.selectionSet->selections[0]);
    EXPECT_EQ(spread.name, "HeroFields");
}

TEST(QueryParser, ParsesInlineFragmentWithTypeCondition) {
    auto ops = ParseOperations("{ hero { ... on Droid { primaryFunction } } }");
    const auto& hero = asField(ops[0].selectionSet.selections[0]);
    ASSERT_EQ(hero.selectionSet->selections.size(), 1);
    const auto& inlineFrag = asInlineFragment(hero.selectionSet->selections[0]);
    ASSERT_TRUE(inlineFrag.typeCondition.has_value());
    EXPECT_EQ(*inlineFrag.typeCondition, "Droid");
    ASSERT_TRUE(inlineFrag.selectionSet.has_value());
    EXPECT_EQ(asField(inlineFrag.selectionSet->selections[0]).name, "primaryFunction");
}

TEST(QueryParser, ParsesInlineFragmentWithoutTypeCondition) {
    auto ops = ParseOperations("{ hero { ... { id } } }");
    const auto& hero = asField(ops[0].selectionSet.selections[0]);
    ASSERT_EQ(hero.selectionSet->selections.size(), 1);
    const auto& inlineFrag = asInlineFragment(hero.selectionSet->selections[0]);
    EXPECT_FALSE(inlineFrag.typeCondition.has_value());
}

TEST(QueryParser, ParsesFieldDirectiveWithBoolArg) {
    auto ops = ParseOperations("query($skip: Boolean!) { hero @skip(if: $skip) }");
    const auto& field = asField(ops[0].selectionSet.selections[0]);
    ASSERT_EQ(field.directives.size(), 1);
    EXPECT_EQ(field.directives[0].name, "skip");
    ASSERT_EQ(field.directives[0].args.size(), 1);
    EXPECT_EQ(field.directives[0].args[0].name, "if");
    EXPECT_EQ(field.directives[0].args[0].value, "$skip");
}

TEST(QueryParser, ParsesFieldDirectiveWithLiteralBoolArg) {
    auto ops = ParseOperations("{ hero @include(if: true) }");
    const auto& field = asField(ops[0].selectionSet.selections[0]);
    ASSERT_EQ(field.directives.size(), 1);
    EXPECT_EQ(field.directives[0].name, "include");
    EXPECT_EQ(field.directives[0].args[0].value, "true");
}

TEST(QueryParser, FieldWithNoDirectivesHasEmptyVector) {
    auto ops = ParseOperations("{ hero }");
    const auto& field = asField(ops[0].selectionSet.selections[0]);
    EXPECT_TRUE(field.directives.empty());
}

TEST(QueryParser, ParsesMultipleDirectivesOnField) {
    auto ops = ParseOperations("{ hero @skip(if: true) @include(if: false) }");
    const auto& field = asField(ops[0].selectionSet.selections[0]);
    ASSERT_EQ(field.directives.size(), 2);
    EXPECT_EQ(field.directives[0].name, "skip");
    EXPECT_EQ(field.directives[1].name, "include");
}

TEST(QueryParser, ParsesDirectiveOnFragmentSpread) {
    auto ops = ParseOperations("{ hero { ...HeroFields @skip(if: true) } }");
    const auto& hero = asField(ops[0].selectionSet.selections[0]);
    const auto& spread = asFragmentSpread(hero.selectionSet->selections[0]);
    ASSERT_EQ(spread.directives.size(), 1);
    EXPECT_EQ(spread.directives[0].name, "skip");
}

TEST(QueryParser, ParsesDirectiveOnInlineFragment) {
    auto ops = ParseOperations("{ hero { ... on Droid @skip(if: true) { id } } }");
    const auto& hero = asField(ops[0].selectionSet.selections[0]);
    const auto& frag = asInlineFragment(hero.selectionSet->selections[0]);
    ASSERT_EQ(frag.directives.size(), 1);
    EXPECT_EQ(frag.directives[0].name, "skip");
}

TEST(QueryParser, ParseDocumentReturnsOperations) {
    auto doc = ParseDocument("{ hello }");
    ASSERT_EQ(doc.operations.size(), 1);
    EXPECT_EQ(doc.operations[0].type._value, OperationType::QUERY);
    ASSERT_EQ(doc.operations[0].selectionSet.selections.size(), 1);
    EXPECT_EQ(asField(doc.operations[0].selectionSet.selections[0]).name, "hello");
}

TEST(QueryParser, ParseDocumentParsesTypedArguments) {
    auto doc = ParseDocument(R"({ user(id: "123", active: true) { name } })");
    auto& field = asField(doc.operations[0].selectionSet.selections[0]);
    auto id = FindArg(field, "id");
    auto active = FindArg(field, "active");
    ASSERT_TRUE(id.has_value());
    ASSERT_TRUE(active.has_value());
    EXPECT_EQ(id->get<std::string>(), "123");
    EXPECT_EQ(active->get<bool>(), true);
}

TEST(QueryParser, ParseDocumentVariableIsReference) {
    auto doc = ParseDocument("query($id: ID!) { user(id: $id) { name } }");
    auto& field = asField(doc.operations[0].selectionSet.selections[0]);
    auto it = std::ranges::find_if(field.arguments, [](const auto& a) { return a.name == "id"; });
    ASSERT_NE(it, field.arguments.end());
    EXPECT_TRUE(it->IsVariable());
    EXPECT_EQ(it->Reference(), "id");
}

TEST(QueryParser, ParseDocumentPopulatesFragmentsMap) {
    auto doc = ParseDocument(R"(
        query { user { ...UserFields } }
        fragment UserFields on User { name email }
    )");
    auto& user = asField(doc.operations[0].selectionSet.selections[0]);
    ASSERT_TRUE(user.selectionSet.has_value());
    auto& spread = std::get<FragmentSpread>(user.selectionSet->selections[0]);
    EXPECT_EQ(spread.name, "UserFields");
    ASSERT_EQ(doc.fragments.size(), 1);
    ASSERT_TRUE(doc.fragments.contains("UserFields"));
    EXPECT_EQ(doc.fragments.at("UserFields").typeCondition, "User");
}

TEST(QueryParser, ParseDocumentDeepNesting) {
    auto doc = ParseDocument(R"({
        post {
            id
            author {
                name
                avatar { url }
            }
        }
    })");
    auto& post = asField(doc.operations[0].selectionSet.selections[0]);
    ASSERT_TRUE(post.selectionSet.has_value());
    auto& author = asField(post.selectionSet->selections[1]);
    ASSERT_TRUE(author.selectionSet.has_value());
    auto& avatar = asField(author.selectionSet->selections[1]);
    EXPECT_EQ(avatar.name, "avatar");
}
