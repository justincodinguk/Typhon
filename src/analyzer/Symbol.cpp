//
// Created by justinw on 05/04/2026.
//

#include "Symbol.h"

#include "SymbolTable.h"

std::shared_ptr<VariableSymbol> Symbol::Var(const std::string &name, const std::shared_ptr<Type> &type,
                                            const bool isMutable,
                                            const Visibility visibility) {
    auto variable = std::make_shared<VariableSymbol>();
    variable->name = name;
    variable->type = type;
    variable->isMutable = isMutable;
    variable->visibility = visibility;
    variable->symbolType = SymbolType::VARIABLE;
    return variable;
}

std::shared_ptr<FunctionSymbol> Symbol::Func(const std::string &name, const std::vector<std::shared_ptr<Type> > &params,
                                             const std::shared_ptr<Type> &returnType,
                                             const std::vector<std::string> &generics,
                                             const bool isExtension, const std::shared_ptr<Type> &extensionOn,
                                             const Visibility visibility) {
    const auto func = std::make_shared<FunctionSymbol>();
    func->name = name;
    func->parameterTypes = params;
    func->returnType = returnType;
    func->extensionOn = extensionOn;
    func->isExtension = isExtension;
    func->generics = generics;
    func->visibility = visibility;
    func->symbolType = SymbolType::FUNCTION;
    return func;
}

std::shared_ptr<ClassSymbol> Symbol::Class(const std::string &name, const std::vector<std::string> &generics, const std::shared_ptr<Type> &parent) {
    const auto classSymbol = std::make_shared<ClassSymbol>();
    classSymbol->name = name;
    classSymbol->generics = generics;
    classSymbol->symbolType = SymbolType::CLASS;
    classSymbol->parent = parent;

    const auto memberTable = std::make_shared<SymbolTable>();
    classSymbol->memberTable = memberTable;
    return classSymbol;
}
