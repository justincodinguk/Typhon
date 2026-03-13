//
// Created by justinw on 12/03/2026.
//

#include "Symbol.h"

void SymbolTable::enterScope() {
    scopes.emplace();
}

void SymbolTable::exitScope() {
    scopes.pop();
}

void SymbolTable::define(const std::string &name, Symbol sym) {

}
