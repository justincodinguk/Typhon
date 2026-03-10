//
// Created by justinw on 06/03/2026.
//

#ifndef PYC___ASTNODE_H
#define PYC___ASTNODE_H
#include <memory>
#include <utility>
#include <vector>

#include "Common.h"
#include "../lexer/Token.h"

struct ASTNode {
    virtual ~ASTNode();
};

struct Parameter : ASTNode {
    std::string name, type;

    Parameter(std::string name, std::string type) : name(std::move(name)), type(std::move(type)) {
    }
};

struct Function : ASTNode {
    std::string name, returnType;
    std::vector<Parameter> params;
    std::unique_ptr<ASTNode> body;
    Visibility visibility;

    Function(std::string name, std::string returnType, std::vector<Parameter> params, std::unique_ptr<ASTNode> body,
             const Visibility visibility)
        : name(std::move(name)), returnType(std::move(returnType)), params(std::move(params)), body(std::move(body)),
          visibility(visibility) {
    }
};

struct ExtensionFunction : ASTNode {
    std::string name, returnType, extensionOn;
    std::vector<Parameter> params;
    std::unique_ptr<ASTNode> body;
    Visibility visibility;

    ExtensionFunction(std::string name, std::string returnType, std::string extensionOn, std::vector<Parameter> params,
                      std::unique_ptr<ASTNode> body, const Visibility visibility)
        : name(std::move(name)), returnType(std::move(returnType)), extensionOn(std::move(extensionOn)),
          params(std::move(params)), body(std::move(body)), visibility(visibility) {
    }
};

struct Class : ASTNode {
    std::string name;
    std::vector<Parameter> params;
    std::unique_ptr<ASTNode> body;
    Visibility visibility;

    Class(std::string name, std::vector<Parameter> params, std::unique_ptr<ASTNode> body, const Visibility visibility)
        : name(std::move(name)), params(std::move(params)), body(std::move(body)), visibility(visibility) {
    }
};


struct Expression : ASTNode {
    ~Expression() override = default;
};

struct BinaryExpr : Expression {
    std::unique_ptr<ASTNode> left, right;
    Token op;

    BinaryExpr(Token op, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
        : left(std::move(left)), right(std::move(right)), op(std::move(op)) {
    }
};

struct ConditionalStatement : ASTNode {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<ASTNode> body;
    std::unique_ptr<ASTNode> elseNode;

    ConditionalStatement(std::unique_ptr<Expression> condition, std::unique_ptr<ASTNode> body,
                         std::unique_ptr<ASTNode> elseNode)
        : condition(std::move(condition)), body(std::move(body)), elseNode(std::move(elseNode)) {
    }
};

struct ForStatement : ASTNode {
    std::string identifier;
    std::unique_ptr<Expression> iterable;
    std::unique_ptr<ASTNode> body;

    ForStatement(std::string identifier, std::unique_ptr<Expression> iterable, std::unique_ptr<ASTNode> body)
        : identifier(std::move(identifier)), iterable(std::move(iterable)), body(std::move(body)) {
    };
};

struct WhileStatement : ASTNode {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<ASTNode> body;

    WhileStatement(std::unique_ptr<Expression> condition, std::unique_ptr<ASTNode> body)
        : condition(std::move(condition)), body(std::move(body)) {
    };
};

struct Identifier : Expression {
    std::string name;

    Identifier(std::string name) : name(std::move(name)) {
    };
};

struct UnaryExpression : Expression {
    Token op;
    std::unique_ptr<Expression> right;

    UnaryExpression(Token op, std::unique_ptr<Expression> right) : op(std::move(op)), right(std::move(right)) {
    };
};

struct Literal : Expression {
    Token token;

    Literal(Token token) : token(std::move(token)) {
    };
};

struct FunctionCallExpression : Expression {
    std::unique_ptr<Expression> callee;
    std::vector<std::unique_ptr<Expression>> args;

    FunctionCallExpression(std::unique_ptr<Expression> callee, std::vector<std::unique_ptr<Expression> > args)
        : callee(std::move(callee)), args(std::move(args)) {
    };
};

struct GetExpression : Expression {
    std::unique_ptr<Expression> left;
    std::string propertyName;

    GetExpression(std::unique_ptr<Expression> left, std::string propertyName) : left(std::move(left)),
        propertyName(std::move(propertyName)) {
    };
};

struct VariableExpression : Expression {
    std::string name;

    VariableExpression(std::string name) : name(std::move(name)) {
    };
};

struct CompoundAssignExpression : Expression {
    std::string name;
    Token op;
    std::unique_ptr<Expression> value;

    CompoundAssignExpression(std::string name, Token op, std::unique_ptr<Expression> value) : name(std::move(name)),
        op(std::move(op)), value(std::move(value)) {
    }
};

struct CompoundSetExpression : Expression {
    std::unique_ptr<Expression> object;
    std::string propertyName;
    Token op;
    std::unique_ptr<Expression> value;

    CompoundSetExpression(std::unique_ptr<Expression> object, std::string propertyName, Token op, std::unique_ptr<Expression> value)
        : object(std::move(object)), propertyName(std::move(propertyName)), op(std::move(op)), value(std::move(value)) {
    }
};

struct AssignExpression : Expression {
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> value;

    AssignExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> value) : left(std::move(left)), value(std::move(value)) {}
};

#endif //PYC___ASTNODE_H
