//
// Created by justinw on 10/03/2026.
//

#include "SemanticAnalyzer.h"

#include "errors/errors.h"
#include "parser/ASTNode.h"

SemanticAnalyzer::SemanticAnalyzer() = default;

SemanticAnalyzer::~SemanticAnalyzer() = default;

void SemanticAnalyzer::visit(BinaryExpr *expr) {
    expr->left->accept(this);
    expr->right->accept(this);

    if (expr->left->evaluatedType != expr->right->evaluatedType) {
        throw std::runtime_error("TypeError");
    }
    expr->evaluatedType = expr->right->evaluatedType;
}

void SemanticAnalyzer::visit(ConditionalStatement *expr) {
    expr->condition->accept(this);
    expr->body->accept(this);
    expr->elseNode->accept(this);

    if (expr->condition->evaluatedType != Type::Bool()) {
        throw std::runtime_error("TypeError");
    }
}

void SemanticAnalyzer::visit(ForStatement *expr) {
    expr->identifier->accept(this);
    expr->iterable->accept(this);
    expr->body->accept(this);

    if (expr->identifier->evaluatedType != expr->iterable->evaluatedType) {
        throw std::runtime_error("TypeError");
    }
}

void SemanticAnalyzer::visit(WhileStatement *expr) {
    expr->condition->accept(this);
    expr->body->accept(this);

    if (expr->condition->evaluatedType != Type::Bool()) {
        throw std::runtime_error("TypeError");
    }
}

void SemanticAnalyzer::visit(Identifier *expr) {
}

void SemanticAnalyzer::visit(UnaryExpression *expr) {
    expr->right->accept(this);
    expr->evaluatedType = expr->right->evaluatedType;
}

void SemanticAnalyzer::visit(Literal *expr) {
    switch (expr->token.type) {
        case TokenType::IntLiteral:
            expr->evaluatedType = Type::Int();
            break;
        case TokenType::FloatLiteral:
            expr->evaluatedType = Type::Float();
            break;
        case TokenType::StringLiteral:
            expr->evaluatedType = Type::String();
            break;
        case TokenType::False:
        case TokenType::True:
            expr->evaluatedType = Type::Bool();
            break;
        default: break;
    }
}

void SemanticAnalyzer::visit(FunctionCallExpression *expr) {
    expr->callee->accept(this);
    const auto &args = expr->args;
    for (int i = 0; i < expr->args.size(); ++i) {
        expr->args[i]->accept(this);
        if (auto &argType = *expr->evaluatedType.wrappedTypes[i + 1].get(); argType != args[i]->evaluatedType) {
            throw std::runtime_error("TypeError");
        }
    }
}

void SemanticAnalyzer::visit(GetExpression *expr) {
    expr->left->accept(this);
    const auto &leftType = expr->left->evaluatedType;
    if (const auto classMember = symbolTable.resolveClassMember(leftType.className, expr->propertyName);
        classMember == nullptr) throw std::runtime_error("Illegal Access");
}

void SemanticAnalyzer::visit(Class *expr) {

}
