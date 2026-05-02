#include "SemanticAnalyzer.h"

#include <algorithm>
#include <iostream>

#include "SymbolTable.h"
#include "parser/ASTNode.h"

SemanticAnalyzer::SemanticAnalyzer() {
    const auto table = std::make_shared<SymbolTable>();
    scopes.push_back(table);
}

SemanticAnalyzer::~SemanticAnalyzer() = default;

void SemanticAnalyzer::visit(Class *expr) {
    const auto classTable = std::make_shared<SymbolTable>();

    std::vector<std::shared_ptr<Type> > params;
    std::ranges::transform(expr->params, std::back_inserter(params),
                           [this](const std::unique_ptr<Parameter> &param) {
                               return typeFromNode(param->type.get());
                           });
    scopes.push_back(classTable);
    const auto classMetadata = std::make_shared<ClassMetadata>();
    symbolTable()->setMetadata(classMetadata);
    for (const auto &param: expr->params) {
        param->accept(this);
    }
    expr->body->accept(this);
    scopes.pop_back();
    symbolTable()->defineClass(expr->name, expr->genericParams, params, expr->visibility, nullptr, "", nullptr,
                               classTable);
}

void SemanticAnalyzer::visit(ReturnExpression *expr) {
    expr->value->accept(this);
    if (const auto functionMetadata = symbolTable()->currentMetadata<FunctionMetadata>();
        *functionMetadata->returnType != *expr->value->evaluatedType) {
        exceptionThrower.typeError("Return type mismatch", functionMetadata->returnType, expr->value->evaluatedType);
    }
}

void SemanticAnalyzer::visit(Body *expr) {
    for (const auto &node: expr->children) {
        node->accept(this);
    }
}

void SemanticAnalyzer::visit(AssignExpression *expr) {
    expr->left->accept(this);
    expr->value->accept(this);

    if (expr->left->evaluatedType->kind == TypeKind::UNINITIALIZED) {
        if (expr->value->evaluatedType->kind == TypeKind::UNINITIALIZED)
            exceptionThrower.typeError(
                "Unknown type in value", nullptr, expr->value->evaluatedType);
        expr->left->evaluatedType = expr->value->evaluatedType;
        return;
    }

    if (*expr->left->evaluatedType != *expr->value->evaluatedType) {
        exceptionThrower.typeError("Type mismatch in assignment", expr->left->evaluatedType,
                                   expr->value->evaluatedType);
    }
}

void SemanticAnalyzer::visit(BinaryExpr *expr) {
    expr->left->accept(this);
    expr->right->accept(this);
    if (expr->left->evaluatedType->kind == TypeKind::UNINITIALIZED) {
        if (expr->right->evaluatedType->kind == TypeKind::UNINITIALIZED)
            exceptionThrower.typeError(
                "Unknown type in value", nullptr, expr->right->evaluatedType);
        expr->left->evaluatedType = expr->right->evaluatedType;
        return;
    }

    if (*expr->left->evaluatedType != *expr->right->evaluatedType) {
        exceptionThrower.typeError("Type mismatch in binary expression", expr->left->evaluatedType,
                                   expr->right->evaluatedType);
    }
    expr->evaluatedType = expr->left->evaluatedType;
}

void SemanticAnalyzer::visit(Literal *expr) {
    switch (expr->token.type) {
        case TokenType::IntLiteral:
            expr->evaluatedType = std::make_shared<Type>(Type::Int());
            break;
        case TokenType::FloatLiteral:
            expr->evaluatedType = std::make_shared<Type>(Type::Float());
            break;
        case TokenType::StringLiteral:
            expr->evaluatedType = std::make_shared<Type>(Type::String());
            break;
        case TokenType::Literal:
            expr->evaluatedType = std::make_shared<Type>(Type::Int());
            break;
        default: exceptionThrower.typeError("Unknown literal", nullptr, nullptr);
    }
}

void SemanticAnalyzer::visit(UnaryExpression *expr) {
    expr->right->accept(this);
    expr->evaluatedType = expr->right->evaluatedType;
}

void SemanticAnalyzer::visit(ExtensionFunction *expr) {
    std::vector<std::shared_ptr<Type> > parameters;
    std::ranges::transform(expr->params, std::back_inserter(parameters),
                           [this](const std::unique_ptr<Parameter> &param) {
                               return typeFromNode(param->type.get());
                           });

    auto returnType = typeFromNode(expr->returnType.get());
    auto extensionOn = typeFromNode(expr->extensionOn.get());
    symbolTable()->defineExtension(expr->name, expr->genericParams, parameters, returnType, extensionOn,
                                   expr->visibility);
    const auto functionMetadata = std::make_shared<FunctionMetadata>(expr->name, parameters, returnType);
    const auto extensionTypeSymbol = symbolTable()->lookup(extensionOn->className)->as<ClassSymbol>();
    scopes.push_back(extensionTypeSymbol->memberTable);
    symbolTable()->setMetadata(functionMetadata);
    symbolTable()->enterScope();
    for (const auto &param: expr->params) param->accept(this);
    expr->body->accept(this);
    symbolTable()->exitScope();
    symbolTable()->setMetadata(nullptr);
    scopes.pop_back();
}

void SemanticAnalyzer::visit(CompoundAssignExpression *expr) {
    expr->value->accept(this);
    auto sym = symbolTable()->lookup(expr->name)->as<VariableSymbol>();
    if (sym == nullptr) exceptionThrower.identifierError("Unknown identifier " + expr->name);
    if (*sym->type != *expr->value->evaluatedType) {
        exceptionThrower.typeError("Type mismatch in compound assignment", sym->type, expr->value->evaluatedType);
    }
}

void SemanticAnalyzer::visit(CompoundSetExpression *expr) {
    expr->object->accept(this);
    expr->value->accept(this);

    if (*expr->object->evaluatedType != *expr->value->evaluatedType) {
        exceptionThrower.typeError("Type mismatch in compound assignment", expr->object->evaluatedType,
                                   expr->value->evaluatedType);
    }
}

void SemanticAnalyzer::visit(Function *expr) {
    std::vector<std::shared_ptr<Type> > parameters;
    std::ranges::transform(expr->params, std::back_inserter(parameters),
                           [this](const std::unique_ptr<Parameter> &param) {
                               return typeFromNode(param->type.get());
                           });
    auto returnType = typeFromNode(expr->returnType.get());
    symbolTable()->defineFunction(expr->name, expr->genericParams, parameters, returnType, expr->visibility);
    const auto functionMetadata = std::make_shared<FunctionMetadata>(expr->name, parameters, returnType);
    symbolTable()->enterScope();
    symbolTable()->setMetadata(functionMetadata);
    for (const auto &param: expr->params) param->accept(this);
    expr->body->accept(this);
    symbolTable()->setMetadata(nullptr);
    symbolTable()->exitScope();
}

void SemanticAnalyzer::visit(TypeNode *expr) {
    for (const auto &wrapped: expr->genericParams) wrapped->accept(this);
    const auto sym = symbolTable()->lookup(expr->typeClass);
    if (const auto clazz = dynamic_cast<ClassSymbol *>(sym.get()); clazz == nullptr) {
        exceptionThrower.identifierError("Unknown type " + expr->typeClass);
    }
}

void SemanticAnalyzer::visit(Parameter *expr) {
    expr->type->accept(this);
    symbolTable()->defineVariable(expr->name, typeFromNode(expr->type.get()), false, expr->visibility);
}

void SemanticAnalyzer::visit(Identifier *expr, bool checkExisting) {
    auto checkClassMetadata = symbolTable()->currentMetadata<ClassMetadata>();
    if (checkClassMetadata != nullptr) {
        auto sym = symbolTable()->lookup(expr->name);
        if (sym!=nullptr) {
            expr->addSelf = true;
            expr->evaluatedType = sym->type;
        } else {
            exceptionThrower.identifierError("Unknown identifier " + expr->name);
        }
    } else {
        auto sym = symbolTable()->lookup(expr->name);
        if (checkExisting && sym==nullptr) {
            exceptionThrower.identifierError("Unknown identifier" + expr->name);
        }
        if (sym!=nullptr) {
            expr->evaluatedType = sym->type;
        }
    }
}

void SemanticAnalyzer::visit(GetExpression *expr) {
    expr->left->accept(this);
    const auto leftType = expr->left->evaluatedType->className;
    const auto symbol = symbolTable()->lookupMember(leftType, expr->propertyName);
    if (symbol == nullptr)
        exceptionThrower.identifierError(
            "Unknown property " + expr->propertyName + " in type " + leftType);
    if (symbol->visibility != Visibility::PUBLIC)
        exceptionThrower.visibilityError(
            expr->propertyName, expr->left->evaluatedType);
    expr->evaluatedType = symbol->type;
}

void SemanticAnalyzer::visit(FunctionCallExpression *expr) {
    expr->callee->accept(this);
    const auto calleeType = expr->callee->evaluatedType;
    auto isNotFunction = calleeType->kind != TypeKind::FUNCTION && calleeType->kind != TypeKind::CLASS_SYM;
    const auto doesntOverrideInvoke = symbolTable()->lookupMember(calleeType->className, "invoke")->as<FunctionSymbol>()
                                      == nullptr;
    if (isNotFunction && doesntOverrideInvoke) exceptionThrower.typeError("Identifier "+calleeType->className+" Type is not callable", nullptr, calleeType);
    for (const auto& arg : expr->args) arg->accept(this);
    if (isNotFunction) {
        const auto invoke = symbolTable()->lookupMember(calleeType->className, "invoke")->as<FunctionSymbol>();
        if (invoke->parameterTypes.size() != expr->args.size()) {
            exceptionThrower.identifierError("Invalid number of arguments for constructor of "+calleeType->className);
        }
        for (size_t i = 0; i < invoke->parameterTypes.size(); i++) {
            if (*expr->args[i]->evaluatedType != *invoke->parameterTypes[i]) {
                exceptionThrower.typeError("Argument type mismatch", invoke->parameterTypes[i], expr->args[i]->evaluatedType);
            }
        }
        expr->evaluatedType = invoke->returnType;
    } else {
        if (calleeType->wrappedTypes.size()-1 != expr->args.size()) {
            exceptionThrower.identifierError("Invalid number of arguments for function call");
        }
        for (size_t i=0;i<expr->args.size();++i) {
            if (*expr->args[i]->evaluatedType != *calleeType->wrappedTypes[i+1]) {
                exceptionThrower.typeError("Argument type mismatch",  calleeType->wrappedTypes[i], expr->args[i]->evaluatedType);
            }
        }
        expr->evaluatedType = calleeType->wrappedTypes.front();
    }
    std::cout << expr->evaluatedType << "\n";

}

void SemanticAnalyzer::visit(ConditionalStatement *expr) {
    expr->condition->accept(this);
    if (expr->condition->evaluatedType->kind != TypeKind::BOOL)
        exceptionThrower.typeError(
            "Expected boolean condition inside if statement", nullptr, expr->condition->evaluatedType);
    symbolTable()->enterScope();
    expr->body->accept(this);
    symbolTable()->exitScope();
    symbolTable()->enterScope();
    expr->elseNode->accept(this);
    symbolTable()->exitScope();
}

void SemanticAnalyzer::visit(WhileStatement *expr) {
    expr->condition->accept(this);
    if (expr->condition->evaluatedType->kind != TypeKind::BOOL)
        exceptionThrower.typeError(
            "Expected boolean condition inside while statement", nullptr, expr->condition->evaluatedType);
    symbolTable()->enterScope();
    expr->body->accept(this);
    symbolTable()->exitScope();
}

void SemanticAnalyzer::visit(ForStatement *expr) {
    expr->iterable->accept(this);
    symbolTable()->enterScope();
    // TODO: Check if iterable is an iterable type
    if (expr->identifier->evaluatedType->kind == TypeKind::UNINITIALIZED) {
        expr->identifier->evaluatedType = expr->iterable->evaluatedType->wrappedTypes.front();
    }
    expr->identifier->accept(this);
    expr->body->accept(this);
    symbolTable()->exitScope();
}

void SemanticAnalyzer::visit(ExternNode *node) {
    node->body->accept(this);
}

void SemanticAnalyzer::analyze(const std::vector<std::unique_ptr<ASTNode> > &nodes) {
    for (auto &node: nodes) node->accept(this);

}

std::shared_ptr<Symbol> SemanticAnalyzer::lookupSymbol(const std::string &name, const std::string &clazz) {
    return symbolTable()->lookupMember(clazz, name);
}

std::shared_ptr<Type> SemanticAnalyzer::typeFromNode(TypeNode *typeNode) {
    std::vector<std::shared_ptr<Type> > wrappedTypes;
    for (const auto &type: typeNode->genericParams) {
        auto parsedType = typeFromNode(type.get());
        wrappedTypes.push_back(parsedType);
    }
    return std::make_shared<Type>(TypeKind::CLASS, typeNode->typeClass, wrappedTypes);
}

std::shared_ptr<SymbolTable> SemanticAnalyzer::symbolTable() {
    return scopes.back();
}
