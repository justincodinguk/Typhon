//
// Created by justinw on 12/03/2026.
//

#ifndef TYPHON_VISITOR_H
#define TYPHON_VISITOR_H

struct BinaryExpr;
struct ConditionalStatement;
struct ForStatement;
struct WhileStatement;
struct Identifier;
struct UnaryExpression;
struct Literal;
struct FunctionCallExpression;
struct GetExpression;
struct VariableExpression;
struct CompoundAssignExpression;
struct CompoundSetExpression;
struct AssignExpression;
struct Class;
struct Function;
struct TypeNode;
struct ExtensionFunction;
struct Parameter;

class Visitor {
public:
    virtual ~Visitor();

    virtual void visit(BinaryExpr* expr) = 0;
    virtual void visit(ConditionalStatement* expr) = 0;
    virtual void visit(ForStatement* expr) = 0;
    virtual void visit(WhileStatement* expr) = 0;
    virtual void visit(Identifier* expr) = 0;
    virtual void visit(UnaryExpression* expr) = 0;
    virtual void visit(Literal* expr) = 0;
    virtual void visit(FunctionCallExpression* expr) = 0;
    virtual void visit(GetExpression* expr) = 0;
    virtual void visit(VariableExpression* expr) = 0;
    virtual void visit(CompoundAssignExpression* expr) = 0;
    virtual void visit(CompoundSetExpression* expr) = 0;
    virtual void visit(AssignExpression* expr) = 0;
    virtual void visit(TypeNode* expr) = 0;
    virtual void visit(Class* expr) = 0;
    virtual void visit(Function* expr) = 0;
    virtual void visit(ExtensionFunction* expr) = 0;
    virtual void visit(Parameter* expr) = 0;
};

#endif //TYPHON_VISITOR_H