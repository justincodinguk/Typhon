//
// Created by justinw on 05/04/2026.
//

#ifndef TYPHON_SYMBOLTABLE_H
#define TYPHON_SYMBOLTABLE_H
#include <unordered_map>

#include "Metadata.h"
#include "Symbol.h"
#include "types/Type.h"

struct ScopedMap {
    std::unordered_map<std::string, std::shared_ptr<Symbol> > map;
    std::shared_ptr<Metadata> metadata;
};

class SymbolTable {
private:
    std::vector<ScopedMap> symbols;

public:
    SymbolTable();

    ~SymbolTable();

    void enterScope();

    void exitScope();

    void defineVariable(const std::string &name, const std::shared_ptr<Type> &type, bool isMutable,
                        Visibility visibility, const std::string &parent = "");

    void defineFunction(const std::string &name, const std::vector<std::string> &generics,
                        const std::vector<std::shared_ptr<Type> > &params,
                        const std::shared_ptr<Type> &returnType, Visibility visibility, const std::string &parent = "");

    void defineFunction(const std::shared_ptr<FunctionSymbol> &function, const std::string &parent = "");

    void defineExtension(const std::string &name, const std::vector<std::string> &generics,
                         const std::vector<std::shared_ptr<Type> > &params,
                         const std::shared_ptr<Type> &returnType, const std::shared_ptr<Type> &extensionOn,
                         Visibility visibility, const std::string &parent = "");

    void defineClass(const std::string &name, const std::vector<std::string> &generics,
                     const std::vector<std::shared_ptr<Type> > &constructorParams, Visibility visibility,
                     const std::shared_ptr<Type> &parentClass, const std::string &parent,
                     const std::shared_ptr<FunctionSymbol> &init, const std::shared_ptr<SymbolTable> &table);

    std::shared_ptr<Symbol> lookup(const std::string &name);

    std::shared_ptr<Symbol> lookupCurrentScope(const std::string &name);

    std::shared_ptr<Symbol> lookupMember(const std::string &clazz, const std::string &name);

    template<typename T> requires std::derived_from<T, Metadata>
    std::shared_ptr<T> currentMetadata() {
        if (auto cast = std::dynamic_pointer_cast<T>(symbols.back().metadata); cast != nullptr) {
            return cast;
        }
        return nullptr;
    }

    void setMetadata(const std::shared_ptr<Metadata> &metadata);
};


#endif //TYPHON_SYMBOLTABLE_H
