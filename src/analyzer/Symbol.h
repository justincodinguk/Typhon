//
// Created by justinw on 05/04/2026.
//

#ifndef TYPHON_SYMBOL_H
#define TYPHON_SYMBOL_H

#include "common/Common.h"
#include "types/Type.h"

struct VariableSymbol;
struct FunctionSymbol;
struct ClassSymbol;
class SymbolTable;

enum class SymbolType {
    VARIABLE, FUNCTION, CLASS, PARAMETER, GENERIC_TYPE
};

struct Symbol {
    std::string name;
    std::shared_ptr<Type> type;
    SymbolType symbolType;
    Visibility visibility;


    virtual ~Symbol() = default;

    template<typename T>
        requires std::derived_from<T, Symbol>
    T *as() {
        return dynamic_cast<T *>(this);
    }

    static std::shared_ptr<VariableSymbol> Var(const std::string &name, const std::shared_ptr<Type> &type, bool isMutable,
                                               Visibility visibility);

    static std::shared_ptr<FunctionSymbol> Func(const std::string &name, const std::vector<std::shared_ptr<Type>> &params,
                                                const std::shared_ptr<Type> &returnType, const std::vector<std::string> &generics,
                                                bool isExtension, const std::shared_ptr<Type> &extensionOn,
                                                Visibility visibility);

    static std::shared_ptr<ClassSymbol> Class(const std::string &name, const std::vector<std::string> &generics,
                                              const std::shared_ptr<Type> &parent);
};

struct VariableSymbol : public Symbol {
    bool isMutable;
};

struct FunctionSymbol : public Symbol {
    std::vector<std::shared_ptr<Type> > parameterTypes;
    std::shared_ptr<Type> returnType;
    std::vector<std::string> generics;
    bool isExtension;
    std::shared_ptr<Type> extensionOn;
};

struct ClassSymbol : public Symbol {
    std::string name;
    std::shared_ptr<SymbolTable> memberTable;
    std::vector<std::string> generics;
    std::shared_ptr<Type> parent;
};

#endif //TYPHON_SYMBOL_H
