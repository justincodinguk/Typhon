//
// Created by justinw on 12/03/2026.
//

#include "Type.h"

Type Type::Int() {
    std::vector<std::unique_ptr<Type> > wrappedTypes;
    return {TypeKind::INT, "", wrappedTypes};
}

Type Type::Float() {
    std::vector<std::unique_ptr<Type> > wrappedTypes;
    return {TypeKind::FLOAT, "", wrappedTypes};
}

Type Type::String() {
    std::vector<std::unique_ptr<Type> > wrappedTypes;
    return {TypeKind::STRING, "", wrappedTypes};
}

Type Type::Bool() {
    std::vector<std::unique_ptr<Type> > wrappedTypes;
    return {TypeKind::BOOL, "", wrappedTypes};
}

Type Type::None() {
    std::vector<std::unique_ptr<Type> > wrappedTypes;
    return {TypeKind::NONE, "", wrappedTypes};
}

Type Type::Class(std::string className, std::vector<std::unique_ptr<Type> > wrappedTypes) {
    return {TypeKind::CLASS, std::move(className), wrappedTypes};
}

Type Type::Function(std::unique_ptr<Type> returnType, std::vector<std::unique_ptr<Type> > paramTypes) {
    std::vector<std::unique_ptr<Type> > vec;
    vec.push_back(std::move(returnType));
    vec.insert(vec.end(), std::make_move_iterator(paramTypes.begin()), std::make_move_iterator(paramTypes.end()));
    return {TypeKind::FUNCTION, "", vec};
}

Type Type::Any() {
    std::vector<std::unique_ptr<Type> > wrappedTypes;
    return {TypeKind::ANY, "", wrappedTypes};
}

Type Type::Error() {
    std::vector<std::unique_ptr<Type> > wrappedTypes;
    return {TypeKind::ERROR, "", wrappedTypes};
}

Type Type::Uninitialized() {
    std::vector<std::unique_ptr<Type> > wrappedTypes;
    return {TypeKind::UNINITIALIZED, "", wrappedTypes};
}

bool Type::operator==(const Type &other) const {
    if (kind != other.kind) return false;
    if (kind == TypeKind::CLASS || kind == TypeKind::FUNCTION) {
        if (kind == TypeKind::CLASS && className != other.className) return false;
        if (wrappedTypes.size() != other.wrappedTypes.size()) return false;
        for (int i = 0; i < wrappedTypes.size(); ++i) {
            if (*wrappedTypes[i] != *other.wrappedTypes[i]) return false;
        }
    }
    return true;
}
