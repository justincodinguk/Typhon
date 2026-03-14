//
// Created by justinw on 12/03/2026.
//

#include "Type.h"

Type Type::Int() {
    return {TypeKind::INT};
}

Type Type::Float() {
    return {TypeKind::FLOAT};
}

Type Type::String() {
    return {TypeKind::STRING};
}

Type Type::Bool() {
    return {TypeKind::BOOL};
}

Type Type::None() {
    return {TypeKind::NONE};
}

Type Type::Class(std::string className, std::vector<std::shared_ptr<Type>> wrappedTypes) {
    return {TypeKind::CLASS, std::move(className), std::move(wrappedTypes)};
}

Type Type::Function(std::shared_ptr<Type> returnType, std::vector<std::shared_ptr<Type>> paramTypes) {
    std::vector vec = {std::move(returnType)};
    vec.insert(vec.end(), paramTypes.begin(), paramTypes.end());
    return {TypeKind::FUNCTION, "", std::move(vec)};
}

Type Type::Any() {
    return {TypeKind::ANY};
}

Type Type::Error() {
    return {TypeKind::ERROR};
}

Type Type::Uninitialized() {
    return {TypeKind::UNINITIALIZED};
}

bool Type::operator==(const Type &other) const {
    if (kind != other.kind) return false;
    if (kind==TypeKind::CLASS || kind==TypeKind::FUNCTION) {
        if (kind==TypeKind::CLASS && className != other.className) return false;
        if (wrappedTypes.size() != other.wrappedTypes.size()) return false;
        for (int i=0;i<wrappedTypes.size();++i) {
            if (*wrappedTypes[i] != *other.wrappedTypes[i]) return false;
        }
    }
    return true;
}