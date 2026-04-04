//
// Created by justinw on 29/03/2026.
//

#include <gtest/gtest.h>
#include <fstream>
#include "lexer/Token.h"
#include "lexer/Lexer.h"

TEST(Lexing, SingleStatementRaw) {
    Lexer lexer;
    const std::string statement = "a: SomeClass[SomeGeneric] = function(\"Hello\").do()";
    const auto tokens = Lexer::tokenize_raw(statement);
    std::vector<std::string> expectedOutput = {
        "a", ":", "SomeClass", "[", "SomeGeneric", "]", "=", "function", "(", "\"Hello\"", ")", ".", "do", "(", ")"
    };
    ASSERT_EQ(expectedOutput.size(), tokens.size());
    ASSERT_EQ(expectedOutput, tokens);
}

TEST(Lexing, SingleStatement) {
    Lexer lexer;
    std::string statement = "a = function(\"Hello\").do()";
    const auto tokens = lexer.tokenize(statement);
    for (auto i : tokens) std::cout << i.toString() << "\n";
    const std::vector<Token> expectedOutput = {
        {TokenType::Identifier, "a", 1, 1},
        {TokenType::Assign, "=", 1, 2},
        {TokenType::Identifier, "function", 1, 3},
        {TokenType::LParen, "(", 1, 4},
        {TokenType::Literal, "\"Hello\"", 1, 5},
        {TokenType::RParen, ")", 1, 6},
        {TokenType::Dot, ".", 1, 7},
        {TokenType::Identifier, "do", 1, 8},
        {TokenType::LParen, "(", 1, 9},
        {TokenType::RParen, ")", 1, 10},
        {TokenType::EndOfFile, std::monostate(), 1, 11}
    };
    ASSERT_EQ(expectedOutput.size(), tokens.size());
    ASSERT_EQ(expectedOutput, tokens);
}

TEST(Lexing, MultipleStatementsRaw) {
    Lexer lexer;
    const std::string file = "def something(a: int, b: list[int]) -> bool {"
            "   for i in b {"
            "        if a==b { return True }"
            "   }"
            "   return False"
            "}";
    const auto tokens = Lexer::tokenize_raw(file);
    const std::vector<std::string> expectedOutput = {
        "def", "something", "(", "a", ":", "int", ",", "b", ":", "list", "[", "int", "]", ")", "->", "bool", "{",
        "for", "i", "in", "b", "{",
        "if", "a", "==", "b", "{", "return", "True", "}", "}", "return", "False", "}"
    };
    ASSERT_EQ(expectedOutput.size(), tokens.size());
    ASSERT_EQ(expectedOutput, tokens);
}

TEST(Lexing, MultipleStatements) {
    Lexer lexer;
    std::string file = "def something(a: int, b: list[int]) -> bool {"
            "   for i in b {"
            "        if a==b { return True }"
            "   }"
            "   return False"
            "}";
    const auto tokens = lexer.tokenize(file);
    const std::vector<Token> expectedOutput = {
        {TokenType::Def, "", 1, 1},
        {TokenType::Identifier, "", 1, 2},
        {TokenType::LParen, "", 1, 3},
        {TokenType::Identifier, "", 1, 4},
        {TokenType::Colon, "", 1, 5},
        {TokenType::Identifier, "", 1, 6},
        {TokenType::Comma, "", 1, 7},
        {TokenType::Identifier, "", 1, 8},
        {TokenType::Colon, "", 1, 9},
        {TokenType::Identifier, "", 1, 10},
        {TokenType::LBracket, "", 1, 11},
        {TokenType::Identifier, "", 1, 12},
        {TokenType::RBracket, "", 1, 13},
        {TokenType::RParen, "", 1, 14},
        {TokenType::Arrow, "", 1, 15},
        {TokenType::Identifier, "", 1, 16},
        {TokenType::LBrace, "", 1, 17},
        {TokenType::For, "", 2, 1},
        {TokenType::Identifier, "", 2, 2},
        {TokenType::In, "", 2, 3},
        {TokenType::Identifier, "", 2, 4},
        {TokenType::LBrace, "", 2, 5},
        {TokenType::If, "", 3, 1},
        {TokenType::Identifier, "", 3, 2},
        {TokenType::Equal, "", 3, 3},
        {TokenType::Identifier, "", 3, 4},
        {TokenType::LBrace, "", 3, 5},
        {TokenType::Return, "", 3, 6},
        {TokenType::True, "", 3, 7},
        {TokenType::RBrace, "", 3, 8},
        {TokenType::RBrace, "", 4, 9},
        {TokenType::Return, "", 5, 1},
        {TokenType::False, "", 5, 2},
        {TokenType::RBrace, "", 6, 1},
        {TokenType::EndOfFile, std::monostate(), 6, 2}
    };

    ASSERT_EQ(expectedOutput.size(), tokens.size());
    ASSERT_EQ(expectedOutput, tokens);
}

TEST(Lexing, FileTest) {
    std::ifstream sample("ty_samples/expressions.ty");
    std::string contents((std::istreambuf_iterator(sample)), std::istreambuf_iterator<char>());

    Lexer lexer;

    auto rawTokens = lexer.tokenize_raw(contents);
    auto tokens = lexer.tokenize(contents);

    std::vector<std::string> expectedRawOutput = {
        "a","=","41","\n","print","(","a",")"
    };

    ASSERT_EQ(expectedRawOutput.size(), rawTokens.size());
    ASSERT_EQ(expectedRawOutput, rawTokens);

    std::vector<Token> expectedOutput = {
        {TokenType::Identifier, "a", 1, 1},
        {TokenType::Assign, "=", 1, 2},
        {TokenType::Literal, 41.0, 1, 3},
        {TokenType::Newline, "\n", 1, 4},
        {TokenType::Identifier, "print", 2, 1},
        {TokenType::LParen, "(", 2, 2},
        {TokenType::Identifier, "a", 2, 3},
        {TokenType::RParen, ")", 2, 4},
        {TokenType::EndOfFile, std::monostate(), 6, 2}
    };

    ASSERT_EQ(expectedOutput.size(), tokens.size());
    ASSERT_EQ(expectedOutput, tokens);
}