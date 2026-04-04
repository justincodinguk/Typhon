//
// Created by justinw on 29/03/2026.
//

#include <gtest/gtest.h>
#include <fstream>

#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "utils.h"

TEST(Parsing, FunctionHeader) {
    std::ifstream sample("ty_samples/function.ty");
    std::string contents((std::istreambuf_iterator(sample)), std::istreambuf_iterator<char>());

    Lexer lexer;
    auto tokens = lexer.tokenize(contents);
    ASSERT_NE(tokens.size(), 1);
    Parser parser(tokens);

    auto tree = parser.parse();
    //std::cout << "Parsed tree\n";
    //print_tree(tree, 0);
    std::vector<std::string> genericsFunction;
    auto p1 = std::make_unique<Parameter>("a", std::make_unique<TypeNode>("int"), Visibility::PUBLIC);
    auto p2 = std::make_unique<Parameter>("b", std::make_unique<TypeNode>("str"), Visibility::PUBLIC);

    std::vector<std::unique_ptr<Parameter> > expectedParams;
    expectedParams.push_back(std::move(p1));
    expectedParams.push_back(std::move(p2));

    std::vector<std::unique_ptr<ASTNode> > expectedBody;
    auto body = std::make_unique<Body>(std::move(expectedBody));
    auto returnType = std::make_unique<TypeNode>("bool");

    auto function = std::make_unique<Function>("someFunction", std::move(returnType), genericsFunction,
                                               std::move(expectedParams), std::move(body), Visibility::PUBLIC);

    std::vector<std::unique_ptr<ASTNode> > expectedTree;
    expectedTree.push_back(std::move(function));

    //std::cout << "Expected tree\n";
    //print_tree(expectedTree, 0);
    ASSERT_EQ(expectedTree.size(), tree.size());
    ASSERT_TRUE(eq_tree(expectedTree, tree));
}

TEST(Parsing, ClassHeader) {
    std::ifstream sample("ty_samples/class.ty");
    std::string contents((std::istreambuf_iterator(sample)), std::istreambuf_iterator<char>());

    Lexer lexer;
    auto tokens = lexer.tokenize(contents);
    ASSERT_NE(tokens.size(), 1);
    Parser parser(tokens);
    auto tree = parser.parse();

    auto p1 = std::make_unique<Parameter>("a", std::make_unique<TypeNode>("int"), Visibility::PUBLIC);
    auto p2 = std::make_unique<Parameter>("b", std::make_unique<TypeNode>("str"), Visibility::PUBLIC);

    std::vector<std::unique_ptr<Parameter> > expectedParams;
    expectedParams.push_back(std::move(p1));
    expectedParams.push_back(std::move(p2));

    std::vector<std::string> generics = {"T"};

    std::vector<std::unique_ptr<ASTNode> > bodyChildren;

    auto fReturnType = std::make_unique<TypeNode>("Default");
    std::vector<std::string> genericsFunction;
    std::vector<std::unique_ptr<Parameter> > fExpectedParams;
    std::vector<std::unique_ptr<ASTNode> > fExpectedBodyChildren;
    auto fBody = std::make_unique<Body>(std::move(fExpectedBodyChildren));
    auto function = std::make_unique<Function>("someMethod", std::move(fReturnType), genericsFunction,
                                               std::move(fExpectedParams), std::move(fBody), Visibility::PUBLIC);

    bodyChildren.push_back(std::move(function));

    auto body = std::make_unique<Body>(std::move(bodyChildren));
    auto classPtr = std::make_unique<Class>("SomeClass", std::move(expectedParams), generics, std::move(body),
                                            Visibility::PRIVATE);

    std::vector<std::unique_ptr<ASTNode> > expectedTree;
    expectedTree.push_back(std::move(classPtr));

    ASSERT_EQ(tree.size(), expectedTree.size());
    ASSERT_TRUE(eq_tree(tree, expectedTree));
}

TEST(Parsing, Expressions) {
    std::ifstream sample("ty_samples/expressions.ty");
    std::string contents((std::istreambuf_iterator(sample)), std::istreambuf_iterator<char>());

    Lexer lexer;
    auto tokens = lexer.tokenize(contents);
    ASSERT_NE(tokens.size(), 1);
    Parser parser(tokens);
    auto tree = parser.parse();

    auto identifier = std::make_unique<Identifier>("a");
    auto literal = std::make_unique<Literal>(Token(TokenType::Literal, 41.0, 0, 0));
    auto assign = std::make_unique<AssignExpression>(std::move(identifier), std::move(literal));

    auto printCallee = std::make_unique<Identifier>("print");
    auto strCallee = std::make_unique<Identifier>("str");
    std::vector<std::unique_ptr<Expression> > args;

    auto a1 = std::make_unique<Identifier>("a");
    args.push_back(std::move(a1));

    auto funcCall = std::make_unique<FunctionCallExpression>(std::move(strCallee), std::move(args));
    std::vector<std::unique_ptr<Expression> > printArgs;
    printArgs.push_back(std::move(funcCall));
    auto printCall = std::make_unique<FunctionCallExpression>(std::move(printCallee), std::move(printArgs));

    std::vector<std::unique_ptr<ASTNode> > expectedTree;
    expectedTree.push_back(std::move(assign));
    expectedTree.push_back(std::move(printCall));

    ASSERT_EQ(tree.size(), expectedTree.size());
    ASSERT_TRUE(eq_tree(tree, expectedTree));
}

TEST(Parsing, FirebaseClientTest) {
    std::ifstream sample("ty_samples/FirebaseClient.ty");
    std::string contents((std::istreambuf_iterator(sample)), std::istreambuf_iterator<char>());

    Lexer lexer;
    auto tokens = lexer.tokenize(contents);

    ASSERT_NE(tokens.size(), 1);
    Parser parser(tokens);
    auto tree = parser.parse();

    auto jsonIdentifier = std::make_unique<Identifier>("json");
    auto iJsonSerializer = std::make_unique<Identifier>("json_serializer");
    auto jsonValueGet = std::make_unique<GetExpression>(std::move(iJsonSerializer), "serialize");
    std::vector<std::unique_ptr<Expression> > jsonValueGetArgs;
    auto objectArg = std::make_unique<Identifier>("object");
    jsonValueGetArgs.push_back(std::move(objectArg));
    auto jsonValueGetCall = std::make_unique<FunctionCallExpression>(std::move(jsonValueGet),
                                                                     std::move(jsonValueGetArgs));
    auto jsonValueAssignExpr = std::make_unique<AssignExpression>(std::move(jsonIdentifier), std::move(jsonValueGetCall));

    auto statusIdentifier = std::make_unique<Identifier>("status");
    auto iHttpClient = std::make_unique<Identifier>("http_client");
    auto httpClientPost = std::make_unique<GetExpression>(std::move(iHttpClient), "post");
    std::vector<std::unique_ptr<Expression> > httpClientPostArgs;
    auto addrArg = std::make_unique<Literal>(Token(TokenType::Literal, "https://somewhere", 0, 0));
    auto jsonArg = std::make_unique<Identifier>("json");
    httpClientPostArgs.push_back(std::move(addrArg));
    httpClientPostArgs.push_back(std::move(jsonArg));

    auto httpClientPostCall = std::make_unique<FunctionCallExpression>(std::move(httpClientPost),
                                                                       std::move(httpClientPostArgs));

    std::vector<std::unique_ptr<ASTNode> > expectedCreateDocumentBody;
    expectedCreateDocumentBody.push_back(std::move(jsonValueAssignExpr));
    expectedCreateDocumentBody.push_back(std::move(httpClientPostCall));
    auto createDocBody = std::make_unique<Body>(std::move(expectedCreateDocumentBody));

    std::vector<std::string> fCreateDocGenerics;
    std::vector<std::unique_ptr<Parameter> > fCreateDocParams;
    auto pObject = std::make_unique<Parameter>("object", std::make_unique<TypeNode>("T"), Visibility::PUBLIC);
    fCreateDocParams.push_back(std::move(pObject));

    auto fCreateDocument = std::make_unique<Function>("createDocument", std::make_unique<TypeNode>("HttpStatus"),
                                                      fCreateDocGenerics, std::move(fCreateDocParams),
                                                      std::move(createDocBody), Visibility::PUBLIC);

    std::vector<std::string> generics;
    std::vector<std::unique_ptr<Parameter> > secParams;
    std::vector<std::unique_ptr<ASTNode> > secBodyChildren;

    auto fSomeSecretMethod = std::make_unique<Function>("someSecretMethod", std::make_unique<TypeNode>("Default"),
                                                        generics, std::move(secParams),
                                                        std::make_unique<Body>(std::move(secBodyChildren)),
                                                        Visibility::PRIVATE
    );


    std::vector<std::unique_ptr<Parameter> > params;
    auto pHttpClient = std::make_unique<Parameter>("http_client", std::make_unique<TypeNode>("HttpClient"),
                                                   Visibility::PUBLIC);
    auto pJsonSerializer = std::make_unique<Parameter>("json_serializer", std::make_unique<TypeNode>("JsonSerializer"),
                                                       Visibility::PUBLIC);
    params.push_back(std::move(pHttpClient));
    params.push_back(std::move(pJsonSerializer));

    std::vector<std::string> classGenerics = {"T"};
    std::vector<std::unique_ptr<ASTNode> > classBodyChildren;
    classBodyChildren.push_back(std::move(fCreateDocument));
    classBodyChildren.push_back(std::move(fSomeSecretMethod));
    auto classBody = std::make_unique<Body>(std::move(classBodyChildren));

    auto firebaseClientClass = std::make_unique<Class>("FirebaseClient", std::move(params), classGenerics,
                                                       std::move(classBody), Visibility::PUBLIC);

    auto iA = std::make_unique<Identifier>("a");
    auto iObject = std::make_unique<Identifier>("object");
    std::vector<std::unique_ptr<Expression> > createDocArgs;
    createDocArgs.push_back(std::move(iObject));
    auto iCreateDoc = std::make_unique<Identifier>("createDocument");
    auto createDocCall = std::make_unique<FunctionCallExpression>(std::move(iCreateDoc), std::move(createDocArgs));

    auto iInt = std::make_unique<Identifier>("int");
    std::vector<std::unique_ptr<Expression> > intArgs;
    intArgs.push_back(std::move(createDocCall));
    auto intCall = std::make_unique<FunctionCallExpression>(std::move(iInt), std::move(intArgs));

    auto expr = std::make_unique<AssignExpression>(std::move(iA), std::move(intCall));
    std::vector<std::unique_ptr<ASTNode> > extBodyChildren;
    extBodyChildren.push_back(std::move(expr));
    auto extBody = std::make_unique<Body>(std::move(extBodyChildren));

    std::vector<std::string> extGenerics = {"T"};

    std::vector<std::unique_ptr<TypeNode> > firebaseGenericParams;
    auto gT = std::make_unique<TypeNode>("T");
    firebaseGenericParams.push_back(std::move(gT));

    std::vector<std::unique_ptr<Parameter> > extParams;
    auto pObj = std::make_unique<Parameter>("object", std::make_unique<TypeNode>("T"), Visibility::PUBLIC);
    extParams.push_back(std::move(pObj));

    auto extOn = std::make_unique<TypeNode>("FirebaseClient", std::move(firebaseGenericParams));
    auto extFunc = std::make_unique<ExtensionFunction>("extensionFunction", std::make_unique<TypeNode>("Default"),
                                                       std::move(extOn), std::move(extParams), extGenerics,
                                                       std::move(extBody), Visibility::PUBLIC);

    std::vector<std::unique_ptr<ASTNode> > expectedTree;
    expectedTree.push_back(std::move(firebaseClientClass));
    expectedTree.push_back(std::move(extFunc));

    ASSERT_EQ(tree.size(), expectedTree.size());
    ASSERT_TRUE(eq_tree(tree, expectedTree));
}
