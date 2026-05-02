//
// Created by justinw on 05/04/2026.
//

#include <gtest/gtest.h>
#include <fstream>

#include "analyzer/SemanticAnalyzer.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"

TEST(Sema, GlobalSymbols) {
    std::ifstream sample("ty_samples/expressions.ty");
    std::string contents((std::istreambuf_iterator(sample)), std::istreambuf_iterator<char>());

    Lexer lexer;
    auto tokens = lexer.tokenize(contents);
    Parser parser(tokens);
    auto tree = parser.parse();


}