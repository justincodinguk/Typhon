//
// Created by justinw on 10/03/2026.
//

#include "SemanticAnalyzer.h"

#include "../errors/errors.h"
#include "../parser/ASTNode.h"

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

void SemanticAnalyzer::visit(Identifier *expr) {}

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
    for (const auto& arg : expr->args) arg->accept(this);
    // TODO: Check from symbol table
}

void SemanticAnalyzer::visit(GetExpression *expr) {

}
