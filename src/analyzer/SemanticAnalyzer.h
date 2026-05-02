//
// Created by justinw on 06/04/2026.
//

#ifndef TYPHON_SEMANTICANALYZER_H
#define TYPHON_SEMANTICANALYZER_H
#include <memory>

#include "Symbol.h"
#include "common/Visitor.h"
#include "errors/ExceptionThrower.h"
#include "parser/ASTNode.h"


class SemanticAnalyzer : Visitor {
private:
    ExceptionThrower exceptionThrower;
    std::vector<std::shared_ptr<SymbolTable>> scopes;

    std::shared_ptr<SymbolTable> symbolTable();
    std::shared_ptr<Type> typeFromNode(TypeNode *typeNode);
public:
    SemanticAnalyzer();
    ~SemanticAnalyzer() override;

    void visit(BinaryExpr *expr) override;
    void visit(ConditionalStatement *expr) override;
    void visit(ForStatement *expr) override;
    void visit(WhileStatement *expr) override;
    void visit(Identifier *expr, bool checkExisting) override;
    void visit(UnaryExpression *expr) override;
    void visit(Literal *expr) override;
    void visit(FunctionCallExpression *expr) override;
    void visit(GetExpression *expr) override;
    void visit(CompoundAssignExpression *expr) override;
    void visit(CompoundSetExpression *expr) override;
    void visit(AssignExpression *expr) override;
    void visit(TypeNode *expr) override;
    void visit(Class *expr) override;
    void visit(Function *expr) override;
    void visit(ReturnExpression *expr) override;
    void visit(Parameter *expr) override;
    void visit(Body *expr) override;
    void visit(ExtensionFunction *expr) override;
    void visit(ExternNode *node) override;

    void analyze(const std::vector<std::unique_ptr<ASTNode>>& nodes);
    std::shared_ptr<Symbol> lookupSymbol(const std::string& name, const std::string& clazz="");
};


#endif //TYPHON_SEMANTICANALYZER_H