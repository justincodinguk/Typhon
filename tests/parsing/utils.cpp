//
// Created by justinw on 30/03/2026.
//

#include "utils.h"
#include <iostream>

std::string btos(bool condition) {
    if (condition) return "true";
    return "false";
}

template<typename T>
requires std::derived_from<T, ASTNode>
bool t_eq_tree(std::vector<std::unique_ptr<T>> &t1, std::vector<std::unique_ptr<T>> &t2) {
    if (t1.size() != t2.size()) return false;
    for (size_t i = 0; i < t1.size(); i++) {
        if (!eq(t1[i].get(),t2[i].get())) return false;
    }
    return true;
}

bool eq(ASTNode *n1, ASTNode *n2) {
    if (n1->kind != n2->kind) {
        return false;
    }
    switch (n1->kind) {
        case NodeKind::TYPE: {
            const auto p1 = dynamic_cast<TypeNode *>(n1);
            const auto p2 = dynamic_cast<TypeNode *>(n2);
            return t_eq_tree(p1->genericParams, p2->genericParams) &&
                   p1->typeClass == p2->typeClass;
        }
        case NodeKind::ASSIGN_EXPR: {
            const auto p1 = dynamic_cast<AssignExpression *>(n1);
            const auto p2 = dynamic_cast<AssignExpression *>(n2);
            return eq(p1->left.get(), p2->left.get()) && eq(p1->value.get(), p2->value.get());
        }
        case NodeKind::PARAMETER: {
            const auto p1 = dynamic_cast<Parameter *>(n1);
            const auto p2 = dynamic_cast<Parameter *>(n2);
            return p1->name == p2->name && eq(p1->type.get(), p2->type.get()) && p1->visibility == p2->visibility;
        }
        case NodeKind::BODY: {
            const auto p1 = dynamic_cast<Body *>(n1);
            const auto p2 = dynamic_cast<Body *>(n2);
            return t_eq_tree(p1->children, p2->children);
        }
        case NodeKind::FUNCTION: {
            const auto p1 = dynamic_cast<Function *>(n1);
            const auto p2 = dynamic_cast<Function *>(n2);
            return p1->name == p2->name && p1->genericParams.size() == p2->genericParams.size() && p1->visibility == p2
                   ->visibility &&
                   t_eq_tree(p1->params, p2->params) && eq(p1->returnType.get(), p2->returnType.get()) && eq(p1->body.get(),p2
                   ->body.get());
        }
        case NodeKind::EXT_FUNCTION: {
            const auto p1 = dynamic_cast<ExtensionFunction *>(n1);
            const auto p2 = dynamic_cast<ExtensionFunction *>(n2);

            return p1->name == p2->name && p1->genericParams.size() == p2->genericParams.size() && p1->visibility == p2
                   ->visibility &&
                   t_eq_tree(p1->params, p2->params) && eq(p1->returnType.get(), p2->returnType.get()) &&
                   eq(p1->body.get(), p2->body.get()) && eq(p1->extensionOn.get(), p2->extensionOn.get());
        }
        case NodeKind::CLASS: {
            const auto p1 = dynamic_cast<Class *>(n1);
            const auto p2 = dynamic_cast<Class *>(n2);
            return p1->name == p2->name && p1->visibility == p2->visibility && p1->genericParams == p2->genericParams &&
                   t_eq_tree(p1->params, p2->params) && eq(p1->body.get(), p2->body.get());
        }
        case NodeKind::BINARY_EXPR: {
            const auto p1 = dynamic_cast<BinaryExpr *>(n1);
            const auto p2 = dynamic_cast<BinaryExpr *>(n2);

            return p1->op == p2->op && eq(p1->left.get(), p2->left.get()) && eq(p1->right.get(), p2->right.get());
        }
        case NodeKind::UNARY_EXPR: {
            const auto p1 = dynamic_cast<UnaryExpression *>(n1);
            const auto p2 = dynamic_cast<UnaryExpression *>(n2);

            return p1->op == p2->op && eq(p1->right.get(), p2->right.get());
        }
        case NodeKind::IF: {
            const auto p1 = dynamic_cast<ConditionalStatement *>(n1);
            const auto p2 = dynamic_cast<ConditionalStatement *>(n2);
            return eq(p1->condition.get(), p2->condition.get()) && eq(p1->body.get(), p2->body.get()) && eq(
                       p1->elseNode.get(), p2->elseNode.get());
        }
        case NodeKind::WHILE: {
            const auto p1 = dynamic_cast<WhileStatement *>(n1);
            const auto p2 = dynamic_cast<WhileStatement *>(n2);

            return eq(p1->condition.get(), p2->condition.get()) && eq(p1->body.get(), p2->body.get());
        }
        case NodeKind::FOR: {
            const auto p1 = dynamic_cast<ForStatement *>(n1);
            const auto p2 = dynamic_cast<ForStatement *>(n2);

            return eq(p1->identifier.get(), p2->identifier.get()) && eq(p1->body.get(), p2->body.get()) && eq(
                       p1->iterable.get(), p2->iterable.get());
        }
        case NodeKind::IDENTIFIER: {
            const auto p1 = dynamic_cast<Identifier *>(n1);
            const auto p2 = dynamic_cast<Identifier *>(n2);
            return p1->name == p2->name;
        }
        case NodeKind::LITERAL: {
            const auto p1 = dynamic_cast<Literal *>(n1);
            const auto p2 = dynamic_cast<Literal *>(n2);

            return p1->token == p2->token;
        }
        case NodeKind::FUNC_CALL: {
            const auto p1 = dynamic_cast<FunctionCallExpression *>(n1);
            const auto p2 = dynamic_cast<FunctionCallExpression *>(n2);

            return eq(p1->callee.get(), p2->callee.get()) && t_eq_tree(p1->args, p2->args);
        }
        case NodeKind::GET_EXPR: {
            const auto p1 = dynamic_cast<GetExpression *>(n1);
            const auto p2 = dynamic_cast<GetExpression *>(n2);

            return p1->propertyName == p2->propertyName && eq(p1->left.get(), p2->left.get());
        }
        case NodeKind::COMP_ASSIGN_EXPR: {
            const auto p1 = dynamic_cast<CompoundAssignExpression *>(n1);
            const auto p2 = dynamic_cast<CompoundAssignExpression *>(n2);

            return p1->name == p2->name && eq(p1->value.get(), p2->value.get()) && p1->op == p2->op;
        }
        case NodeKind::COMP_SET_EXPR: {
            const auto p1 = dynamic_cast<CompoundSetExpression *>(n1);
            const auto p2 = dynamic_cast<CompoundSetExpression *>(n2);

            return eq(p1->value.get(), p2->value.get()) && p1->op == p2->op && p1->propertyName == p2->propertyName && eq(p1->object.get(),
                   p2->object.get());
        }
    }
    return false;
}

bool eq_tree(std::vector<std::unique_ptr<ASTNode> > &tree1, std::vector<std::unique_ptr<ASTNode> > &tree2) {
    if (tree1.size() != tree2.size()) return false;
    for (int i = 0; i < tree1.size(); i++) {
        if (!eq(tree1[i].get(), tree2[i].get())) return false;
    }
    return true;
}

void print_line(const std::string &line, int indent) {
    for (int i = 0; i < indent * 4; i++) std::cout << "-";
    std::cout << line << "\n";
}

void print_node(ASTNode *node, int indent) {
    switch (node->kind) {
        case NodeKind::TYPE: {
            const auto p1 = dynamic_cast<TypeNode *>(node);
            print_line("TYPE NODE", indent);
            print_line("Type: " + p1->typeClass, indent);
            for (auto &generic: p1->genericParams) print_node(generic.get(), indent + 1);
            break;
        }
        case NodeKind::PARAMETER: {
            const auto p = dynamic_cast<Parameter *>(node);
            print_line("PARAMETER NODE", indent);
            print_line("Name: " + p->name, indent);
            print_node(p->type.get(), indent + 1);
            break;
        }
        case NodeKind::BODY: {
            const auto p = dynamic_cast<Body *>(node);
            print_line("BODY NODE", indent);
            for (auto &_node: p->children) {
                print_node(_node.get(), indent + 1);
            }
            break;
        }
        case NodeKind::FUNCTION: {
            const auto p = dynamic_cast<Function *>(node);
            print_line("FUNCTION NODE", indent);
            print_line("Name: " + p->name, indent + 1);
            print_line("Generic size: " + std::to_string(p->genericParams.size()), indent + 1);
            print_line("PARAMETERS", indent + 1);
            for (auto &param: p->params) print_node(param.get(), indent + 2);
            print_node(p->returnType.get(), indent + 1);
            print_node(p->body.get(), indent + 1);
            break;
        }
        case NodeKind::EXT_FUNCTION: {
            const auto p = dynamic_cast<ExtensionFunction *>(node);
            print_line("EXT_FUNCTION NODE", indent);
            print_line("Name: " + p->name, indent + 1);
            print_line("On: ", indent + 1);
            print_node(p->extensionOn.get(), indent + 2);
            print_line("Generic size: " + std::to_string(p->genericParams.size()), indent + 1);
            print_line("PARAMETERS", indent + 1);
            for (auto &param: p->params) print_node(param.get(), indent + 2);
            print_line("RETURN: ", indent + 1);
            print_node(p->returnType.get(), indent + 2);
            print_node(p->body.get(), indent + 2);
            break;
        }
        case NodeKind::CLASS: {
            const auto p = dynamic_cast<Class *>(node);
            print_line("CLASS NODE", indent);
            print_line("Name: " + p->name, indent + 1);
            print_line("Generic size: " + std::to_string(p->genericParams.size()), indent + 1);
            print_line("PARAMETERS", indent + 1);
            for (auto &param: p->params) print_node(param.get(), indent + 2);
            print_node(p->body.get(), indent + 2);
            break;
        }
        case NodeKind::BINARY_EXPR: {
            const auto p = dynamic_cast<BinaryExpr *>(node);
            print_line("BINARY NODE", indent);
            print_line("LEFT: ", indent + 1);
            print_node(p->left.get(), indent + 2);
            print_line("Operator: " + p->op.toString(), indent + 1);
            print_line("RIGHT: ", indent + 1);
            print_node(p->right.get(), indent + 2);
            break;
        }
        case NodeKind::UNARY_EXPR: {
            const auto p = dynamic_cast<UnaryExpression *>(node);
            print_line("UNARY NODE", indent);
            print_line("Operator: " + p->op.toString(), indent + 1);
            print_line("RIGHT: ", indent + 1);
            print_node(p->right.get(), indent + 2);
            break;
        }
        case NodeKind::IF: {
            const auto p = dynamic_cast<ConditionalStatement *>(node);
            print_line("IF NODE", indent);
            print_line("Condition: ", indent + 1);
            print_node(p->condition.get(), indent + 2);
            print_node(p->body.get(), indent + 2);
            print_line("ELSE", indent + 1);
            print_node(p->elseNode.get(), indent + 2);
            break;
        }
        case NodeKind::WHILE: {
            const auto p = dynamic_cast<WhileStatement *>(node);
            print_line("WHILE NODE", indent);
            print_line("Condition: ", indent + 1);
            print_node(p->condition.get(), indent + 2);
            print_node(p->body.get(), indent + 2);
            break;
        }
        case NodeKind::FOR: {
            const auto p = dynamic_cast<ForStatement *>(node);
            print_line("FOR NODE", indent);
            print_line("Identifier: ", indent + 1);
            print_node(p->identifier.get(), indent + 2);
            print_line("Iterable: ", indent + 1);
            print_node(p->iterable.get(), indent + 2);
            print_node(p->body.get(), indent + 2);
            break;
        }
        case NodeKind::IDENTIFIER: {
            const auto p = dynamic_cast<Identifier *>(node);
            print_line("IDENTIFIER NODE", indent);
            print_line("Identifier: " + p->name, indent + 1);
            break;
        }
        case NodeKind::LITERAL: {
            const auto p = dynamic_cast<Literal *>(node);
            print_line("LITERAL NODE", indent);
            print_line("Value: " + p->token.toString(), indent + 1);
            break;
        }
        case NodeKind::FUNC_CALL: {
            const auto p = dynamic_cast<FunctionCallExpression *>(node);
            print_line("FUNC_CALL NODE", indent);
            print_line("Callee: ", indent + 1);
            print_node(p->callee.get(), indent + 2);
            print_line("Arguments: ", indent + 1);
            for (auto &arg: p->args) print_node(arg.get(), indent + 2);
            break;
        }
        case NodeKind::GET_EXPR: {
            const auto p = dynamic_cast<GetExpression *>(node);
            print_line("GET_EXPR NODE", indent);
            print_line("Left: ", indent + 1);
            print_node(p->left.get(), indent + 2);
            print_line("Get: " + p->propertyName, indent + 1);
            break;
        }
        case NodeKind::COMP_ASSIGN_EXPR: {
            const auto p = dynamic_cast<CompoundAssignExpression *>(node);
            print_line("COMP_ASSIGN_EXPR NODE", indent);
            print_line("Name: " + p->name, indent + 1);
            print_line("Op: " + p->op.toString(), indent + 1);
            print_line("Value: ", indent + 1);
            print_node(p->value.get(), indent + 2);
            break;
        }
        case NodeKind::COMP_SET_EXPR: {
            const auto p = dynamic_cast<CompoundSetExpression *>(node);
            print_line("COMP_SET_EXPR NODE", indent);
            print_line("Object: ", indent + 1);
            print_node(p->object.get(), indent + 2);
            print_line("Property: " + p->propertyName, indent + 1);
            print_line("Operator: " + p->op.toString(), indent + 1);
            print_line("Value: ", indent + 1);
            print_node(p->value.get(), indent + 2);
            break;
        }
        case NodeKind::ASSIGN_EXPR: {
            const auto p = dynamic_cast<AssignExpression *>(node);
            print_line("ASSIGN_EXPR NODE", indent);
            print_line("Left: ", indent + 1);
            print_node(p->left.get(), indent + 2);
            print_line("Value: ", indent + 1);
            print_node(p->value.get(), indent + 2);
            break;
        }
    }
}

void print_tree(std::vector<std::unique_ptr<ASTNode> > &tree, int indent) {
    for (auto &node: tree) {
        print_node(node.get(), indent);
    }
}
