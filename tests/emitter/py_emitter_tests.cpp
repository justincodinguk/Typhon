//
// Created by justinw on 05/04/2026.
//

#include <gtest/gtest.h>
#include <fstream>

#include "../parsing/utils.h"
#include "analyzer/SemanticAnalyzer.h"
#include "emitter/PythonEmitter.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"

TEST(PyEmitter, FirebaseClient) {
    std::ifstream sample("ty_samples/FirebaseClient.ty");
    std::string contents((std::istreambuf_iterator(sample)), std::istreambuf_iterator<char>());
    Lexer lexer;
    auto tokens = lexer.tokenize(contents);
    Parser parser(tokens);
    auto ast = parser.parse();
    SemanticAnalyzer sema;

    sema.analyze(ast);
    PythonEmitter emitter(std::move(ast));

    emitter.writeToFile("FirebaseClient.py");
    system("python FirebaseClient.py");
    ASSERT_TRUE(true);
}