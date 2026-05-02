#include "Parser.h"

#include <iostream>
#include <ostream>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {
}

Parser::~Parser() = default;

Token Parser::peek() {
    return tokens[index];
}

Token Parser::peek(const int n) {
    if (index + n < tokens.size()) return tokens[index + n];
    return tokens[tokens.size() - 1];
}

Token Parser::consume() {
    if (!isAtEnd()) index++;
    return tokens[index - 1];
}

Token Parser::consume(const TokenType type, const std::string &error) {
    if (match(type)) return peek(-1);
    perror(
        (error + " at line " + std::to_string(tokens[index].line) + " column " + std::to_string(tokens[index].column) +
         "\n" + "Found " + peek().toString() + " instead\n").c_str());
    exit(1);
}

Token Parser::consume(const std::function<bool(TokenType)> &predicate, const std::string &error) {
    if (predicate(peek().type)) return consume();
    perror(
        (error + " at line " + std::to_string(tokens[index].line) + " column " + std::to_string(tokens[index].column) +
         "\n" + "Found " + peek().toString() + "instead\n").c_str());
    exit(1);
}

bool Parser::match(const TokenType type) {
    if (tokens[index].type == type) {
        consume();
        return true;
    }
    return false;
}

bool Parser::isAtEnd() const {
    return tokens[index].type == TokenType::EndOfFile;
}

std::vector<std::unique_ptr<ASTNode> > Parser::parse() {
    auto globalBody = block();
    return std::move(globalBody->children);
}

std::unique_ptr<ASTNode> Parser::classDeclaration(Visibility visibility) {
    consume(TokenType::Class, "Expected class definition");
    Token name = consume(TokenType::Identifier, "Expected class name");
    auto generics = genericParametersDeclaration();
    auto parameters = parameterList();
    consume(TokenType::LBrace, "Expected '{'");
    auto body = block();
    consume(TokenType::RBrace, "Expected '}'");
    return std::make_unique<Class>(name.toString(), std::move(parameters), std::move(generics), std::move(body),
                                   visibility);
}

std::unique_ptr<ASTNode> Parser::functionDeclaration(Visibility visibility) {
    consume(TokenType::Def, "Expected function definition");
    Token name = consume(TokenType::Identifier, "Expected function name");
    auto generics = genericParametersDeclaration();
    auto parameters = parameterList();
    auto returnType = std::make_unique<TypeNode>("Default");
    if (match(TokenType::Arrow)) returnType = typeDeclaration();

    consume(TokenType::LBrace, "Expected '{'");
    auto body = block();
    consume(TokenType::RBrace, "Expected '}'");
    return std::make_unique<Function>(name.toString(), std::move(returnType), std::move(generics),
                                      std::move(parameters), std::move(body), visibility);
}

std::unique_ptr<ASTNode> Parser::extensionFunctionDeclaration(Visibility visibility) {
    consume(TokenType::Ext, "Expected extension function definition");
    consume(TokenType::Def, "Expected extension function definition");
    auto extensionOn = typeDeclaration();
    consume(TokenType::Dot, "Expected '.'");
    Token name = consume(TokenType::Identifier, "Expected extension function name");
    auto generics = genericParametersDeclaration();
    auto parameters = parameterList();
    auto returnType = std::make_unique<TypeNode>("Default");
    if (match(TokenType::Arrow)) {
        returnType = typeDeclaration();
    }
    consume(TokenType::LBrace, "Expected '{'");
    auto body = block();
    consume(TokenType::RBrace, "Expected '}'");
    return std::make_unique<ExtensionFunction>(name.toString(), std::move(returnType), std::move(extensionOn),
                                               std::move(parameters), std::move(generics), std::move(body), visibility);
}

std::vector<std::unique_ptr<Parameter> > Parser::parameterList() {
    consume(TokenType::LParen, "Expected '('");
    std::vector<std::unique_ptr<Parameter> > params;
    if (!match(TokenType::RParen)) {
        params.push_back(parameter());
        while (match(TokenType::Comma)) params.push_back(parameter());
        consume(TokenType::RParen, "Expected ')'");
    }
    return params;
}

std::unique_ptr<Parameter> Parser::parameter() {
    Token name = consume(TokenType::Identifier, "Expected parameter name");
    consume(TokenType::Colon, "Incomplete parameter declaration");
    auto type = typeDeclaration();
    return std::make_unique<Parameter>(name.toString(), std::move(type), Visibility::PUBLIC);
}

std::unique_ptr<ASTNode> Parser::conditionalStatement() {
    consume(TokenType::If, "Expected if");
    auto condition = expression();
    consume(TokenType::LBrace, "Expected '{'");
    auto body = block();
    consume(TokenType::RBrace, "Expected '}'");
    std::unique_ptr<ASTNode> elseNode;
    if (match(TokenType::Else)) {
        if (peek().type == TokenType::If) {
            elseNode = conditionalStatement();
        } else {
            consume(TokenType::LBrace, "Expected '{'");
            elseNode = block();
            consume(TokenType::RBrace, "Expected '}'");
        }
    }
    return std::make_unique<ConditionalStatement>(std::move(condition), std::move(body), std::move(elseNode));
}

std::unique_ptr<ASTNode> Parser::whileStatement() {
    consume(TokenType::While, "Expected while loop declaration");
    auto condition = expression();
    consume(TokenType::LBrace, "Expected '{'");
    auto body = block();
    consume(TokenType::RBrace, "Expected '}'");
    return std::make_unique<WhileStatement>(std::move(condition), std::move(body));
}

std::unique_ptr<ASTNode> Parser::forStatement() {
    consume(TokenType::For, "Expected for loop declaration");
    auto identifier = consume(TokenType::Identifier, "Expected identifier");
    consume(TokenType::In, "Expected in");
    auto iterable = expression();
    consume(TokenType::LBrace, "Expected '{'");
    auto body = block();
    consume(TokenType::RBrace, "Expected '}'");
    return std::make_unique<ForStatement>(std::make_unique<Identifier>(identifier.toString()), std::move(iterable),
                                          std::move(body));
}

std::unique_ptr<Body> Parser::block() {
    std::vector<std::unique_ptr<ASTNode> > children;
    while (peek().type != TokenType::RBrace && peek().type != TokenType::EndOfFile) {
        auto visibilityModifier = Visibility::PUBLIC;
        switch (peek().type) {
            case TokenType::Public:
                consume(TokenType::Public, "Expected visibility modifier");
                break;
            case TokenType::Private:
                consume(TokenType::Private, "Expected visibility modifier");
                visibilityModifier = Visibility::PRIVATE;
                break;
            case TokenType::Internal:
                consume(TokenType::Internal, "Expected visibility modifier");
                visibilityModifier = Visibility::INTERNAL;
                break;
            default: ;
        }

        switch (peek().type) {
            case TokenType::If:
                children.push_back(conditionalStatement());
                break;
            case TokenType::For:
                children.push_back(forStatement());
                break;
            case TokenType::While:
                children.push_back(whileStatement());
                break;
            case TokenType::Def:
                children.push_back(functionDeclaration(visibilityModifier));
                break;
            case TokenType::Ext:
                children.push_back(extensionFunctionDeclaration(visibilityModifier));
                break;
            case TokenType::Class:
                children.push_back(classDeclaration(visibilityModifier));
                break;
            case TokenType::Return:
                children.push_back(returnStatement());
                break;
            case TokenType::Extern:
                children.push_back(externNode());
                break;
            case TokenType::Newline:
                consume();
                break;
            default:
                children.push_back(expression());
                break;
        }
    }
    return std::make_unique<Body>(std::move(children));
}

std::unique_ptr<ASTNode> Parser::returnStatement() {
    consume(TokenType::Return, "Expected return statement");
    auto expr = expression();
    return std::make_unique<ReturnExpression>(std::move(expr));
}

std::vector<std::unique_ptr<TypeNode> > Parser::genericArgumentsDeclaration() {
    std::vector<std::unique_ptr<TypeNode> > generics;
    if (match(TokenType::LBracket)) {
        do {
            auto typeName = consume(TokenType::Identifier, "Expected generic type argument");
            auto recursiveGenerics = genericArgumentsDeclaration();
            auto node = std::make_unique<TypeNode>(typeName.toString(), std::move(recursiveGenerics));
            generics.push_back(std::move(node));
        } while (match(TokenType::Comma));
        consume(TokenType::RBracket, "Expected ']'");
    }
    return generics;
}

std::unique_ptr<ASTNode> Parser::externNode() {
    consume(TokenType::Extern, "Expected extern declaration");
    consume(TokenType::LBrace, "Expected '{'");
    auto body = block();
    consume(TokenType::RBrace, "Expected '}'");

    return std::make_unique<ExternNode>(std::move(body));
}

std::vector<std::string> Parser::genericParametersDeclaration() {
    std::vector<std::string> params;
    if (match(TokenType::LBracket)) {
        do params.push_back(consume(TokenType::Identifier, "Expected generic parameter").toString());
        while (match(TokenType::Comma));

        consume(TokenType::RBracket, "Expected ']'");
    }
    return params;
}

std::unique_ptr<TypeNode> Parser::typeDeclaration() {
    auto typeName = consume(TokenType::Identifier, "Expected type name");
    auto generics = genericArgumentsDeclaration();
    return std::make_unique<TypeNode>(typeName.toString(), std::move(generics));
}

std::unique_ptr<Expression> Parser::expression() {
    return parseExpression(NONE);
}

std::unique_ptr<Expression> Parser::parseExpression(const int precedence) {
    auto left = parsePrefix();
    while (true) {
        if (const int nextPrecedence = getPrecedence(peek().type); nextPrecedence <= precedence || isAtEnd()) break;
        const auto op = consume([](const TokenType type) {
            return type >= TokenType::Dot && type <= TokenType::UnaryPlus;
        }, "Expected operator");
        left = parseInfix(std::move(left), op);
    }
    return left;
}

std::unique_ptr<Expression> Parser::parsePrefix() {
    switch (Token next = consume(); next.type) {
        case TokenType::IntLiteral:
        case TokenType::StringLiteral:
        case TokenType::FloatLiteral:
        case TokenType::Literal:
        case TokenType::True:
        case TokenType::False:
            return std::make_unique<Literal>(next);
        case TokenType::Identifier:
            return std::make_unique<Identifier>(next.toString());
        case TokenType::LParen: {
            auto expr = parseExpression(Precedence::NONE);
            consume(TokenType::RParen, "Expected ')'");
            return expr;
        }
        case TokenType::UnaryMinus:
        case TokenType::UnaryPlus:
        case TokenType::Increment:
        case TokenType::Decrement:
        case TokenType::Bang: {
            auto prefix = parsePrefix();
            return std::make_unique<UnaryExpression>(next, std::move(prefix));
        }
        default:
            throw std::runtime_error("Expected expression");
    }
}

std::unique_ptr<Expression> Parser::parseInfix(std::unique_ptr<Expression> left, Token op) {
    int precedence = getPrecedence(op.type);
    switch (op.type) {
        case TokenType::Dot: {
            auto functionName = consume(TokenType::Identifier, "Expected property name after '.'");
            return std::make_unique<GetExpression>(std::move(left), functionName.toString());
        }

        case TokenType::LParen:
            return finishCall(std::move(left));

        case TokenType::Plus:
        case TokenType::Minus:
        case TokenType::Multiply:
        case TokenType::Divide:
        case TokenType::Modulo:
        case TokenType::FloorDivide:
        case TokenType::Power: {
            auto right = parseExpression(precedence);
            return std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
        }

        case TokenType::PlusAssign:
        case TokenType::MinusAssign:
        case TokenType::MultiplyAssign:
        case TokenType::DivideAssign:
        case TokenType::FloorDivideAssign:
        case TokenType::ModuloAssign:
        case TokenType::PowerAssign: {
            auto value = parseExpression(ASSIGNMENT - 1);
            if (auto *var = dynamic_cast<Identifier *>(left.get())) {
                return std::make_unique<CompoundAssignExpression>(var->name, op, std::move(value));
            }
            if (auto *get = dynamic_cast<GetExpression *>(left.get())) {
                return std::make_unique<CompoundSetExpression>(std::move(get->left), get->propertyName, op,
                                                               std::move(value));
            }
            throw std::runtime_error("Invalid assignment target");
        }
        case TokenType::Assign: {
            auto value = parseExpression(ASSIGNMENT - 1);
            return std::make_unique<AssignExpression>(std::move(left), std::move(value));
        }
        default:
            return left;
    }
}

std::unique_ptr<Expression> Parser::finishCall(std::unique_ptr<Expression> callee) {
    std::vector<std::unique_ptr<Expression> > args;
    if (!match(TokenType::RParen)) {
        do args.push_back(parseExpression(NONE));
        while (match(TokenType::Comma));

        consume(TokenType::RParen, "Expected ')' after arguments");
    }

    return std::make_unique<FunctionCallExpression>(std::move(callee), std::move(args));
}

int Parser::getPrecedence(const TokenType type) {
    switch (type) {
        case TokenType::PlusAssign:
        case TokenType::MinusAssign:
        case TokenType::MultiplyAssign:
        case TokenType::DivideAssign:
        case TokenType::FloorDivideAssign:
        case TokenType::ModuloAssign:
        case TokenType::PowerAssign:
        case TokenType::Assign: return ASSIGNMENT;
        case TokenType::Or: return OR;
        case TokenType::And: return AND;
        case TokenType::Equal:
        case TokenType::NotEqual: return EQUALITY;
        case TokenType::LessThan:
        case TokenType::LessThanOrEqual:
        case TokenType::GreaterThan:
        case TokenType::GreaterThanOrEqual: return COMPARISON;
        case TokenType::Plus:
        case TokenType::Minus: return TERM;
        case TokenType::Multiply:
        case TokenType::Divide:
        case TokenType::FloorDivide:
        case TokenType::Modulo: return FACTOR;
        case TokenType::Power: return EXPONENT;
        case TokenType::LParen:
        case TokenType::Dot: return CALL;
        default: return NONE;
    }
}
