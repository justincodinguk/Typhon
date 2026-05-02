//
// Created by justinw on 05/04/2026.
//

#include "SymbolTable.h"

#include <iostream>

void SymbolTable::enterScope() {
    symbols.emplace_back();
}

void SymbolTable::exitScope() {
    symbols.pop_back();
}

SymbolTable::SymbolTable() {
    enterScope();
}

SymbolTable::~SymbolTable() = default;

void SymbolTable::defineVariable(const std::string &name, const std::shared_ptr<Type> &type, const bool isMutable,
                                 const Visibility visibility, const std::string &parent) {
    const auto var = Symbol::Var(name, type, isMutable, visibility);
    if (parent.empty()) symbols.back().map[name] = var;
    else {
        auto sym = lookupCurrentScope(parent);
        if (const auto classSymbol = sym->as<ClassSymbol>(); classSymbol != nullptr) {
            classSymbol->memberTable->defineVariable(name, type, isMutable, visibility, "");
        }
    }
}

void SymbolTable::defineFunction(const std::string &name, const std::vector<std::string> &generics,
                                 const std::vector<std::shared_ptr<Type> > &params,
                                 const std::shared_ptr<Type> &returnType,
                                 Visibility visibility, const std::string &parent) {
    const auto func = Symbol::Func(name, params, returnType, generics, false, nullptr, visibility);
    const auto funcType = std::make_shared<Type>(Type::Function(returnType, params, const_cast<std::string&>(name)));
    func->type = funcType;
    if (parent.empty()) symbols.back().map[name] = func;
    else {
        auto sym = lookupCurrentScope(parent);
        if (const auto classSymbol = sym->as<ClassSymbol>(); classSymbol != nullptr) {
            classSymbol->memberTable->defineFunction(name, generics, params, returnType, visibility, "");
        }
    }
}

void SymbolTable::defineFunction(const std::shared_ptr<FunctionSymbol>& function, const std::string &parent) {
    auto funcType = std::make_shared<Type>(Type::Function(function->returnType, function->parameterTypes, function->name));
    function->type = funcType;
    if (parent.empty()) symbols.back().map[function->name] = function;
    else {
        const auto sym = lookupCurrentScope(parent);
        if (const auto classSymbol = sym->as<ClassSymbol>(); classSymbol != nullptr) {
            classSymbol->memberTable->defineFunction(function, "");
        }
    }
}

void SymbolTable::defineExtension(const std::string &name, const std::vector<std::string> &generics,
                                  const std::vector<std::shared_ptr<Type> > &params,
                                  const std::shared_ptr<Type> &returnType, const std::shared_ptr<Type> &extensionOn,
                                  const Visibility visibility, const std::string &parent) {
    const auto func = Symbol::Func(name, params, returnType, generics, true, extensionOn, visibility);
    if (parent.empty()) symbols.back().map[name] = func;
    else {
        auto sym = lookupCurrentScope(parent);
        if (const auto classSymbol = sym->as<ClassSymbol>(); classSymbol != nullptr) {
            classSymbol->memberTable->defineExtension(name, generics, params, returnType, extensionOn, visibility, "");
        }
    }
}

void SymbolTable::defineClass(const std::string &name, const std::vector<std::string> &generics,
                              const std::vector<std::shared_ptr<Type> > &constructorParams, Visibility visibility,
                              const std::shared_ptr<Type> &parentClass, const std::string &parent,
                              const std::shared_ptr<FunctionSymbol> &init, const std::shared_ptr<SymbolTable>& table) {
    if (parent.empty()) {
        const auto clazz = Symbol::Class(name, generics, parentClass);
        clazz->memberTable = table;
        if (init!=nullptr) clazz->memberTable->defineFunction(init);
        symbols.back().map[name] = clazz;
        std::vector<std::shared_ptr<Type>> wrapped;
        auto clazzType = std::make_shared<Type>(Type::Class(name, {}));
        wrapped.push_back(clazzType);
        wrapped.insert_range(wrapped.end(), constructorParams);
        clazz->type = std::make_shared<Type>(Type::ClassSymbol(name, wrapped));
    }
    else {
        const auto sym = lookupCurrentScope(parent);
        if (const auto classSymbol = sym->as<ClassSymbol>(); classSymbol != nullptr) {
            classSymbol->memberTable->defineClass(name, generics, constructorParams, visibility, parentClass, "", init, table);
        }
    }
}

std::shared_ptr<Symbol> SymbolTable::lookup(const std::string& name) {
    for (int i=symbols.size()-1; i>=0; --i) {
        if (symbols[i].map.contains(name)) return symbols[i].map.at(name);
    }
    return nullptr;
}

std::shared_ptr<Symbol> SymbolTable::lookupCurrentScope(const std::string& name) {
    if (symbols.back().map.contains(name)) return symbols.back().map.at(name);
    return nullptr;
}

std::shared_ptr<Symbol> SymbolTable::lookupMember(const std::string& clazz, const std::string& name) {
    const auto symbol = lookup(clazz);
    if (const auto classSymbol = symbol->as<ClassSymbol>(); classSymbol != nullptr) {
        return classSymbol->memberTable->lookup(name);
    }
    return nullptr;
}

void SymbolTable::setMetadata(const std::shared_ptr<Metadata>& metadata) {
    symbols.back().metadata = metadata;
}
