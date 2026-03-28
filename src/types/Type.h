//
// Created by justinw on 12/03/2026.
//

#ifndef TYPHON_TYPE_H
#define TYPHON_TYPE_H
#include <memory>
#include <string>
#include <vector>

enum class TypeKind {
    INT, FLOAT, STRING, BOOL, NONE, CLASS, FUNCTION, ERROR, ANY, UNINITIALIZED
};

struct Type {
    TypeKind kind;
    std::string className;
    std::vector<std::unique_ptr<Type>>& wrappedTypes;

    static Type Int();
    static Type Float();
    static Type String();
    static Type Bool();
    static Type None();
    static Type Class(std::string className, std::vector<std::unique_ptr<Type>> wrappedTypes);
    static Type Function(std::unique_ptr<Type> returnType, std::vector<std::unique_ptr<Type>> paramTypes);
    static Type Any();
    static Type Error();
    static Type Uninitialized();

    bool operator==(const Type &other) const;
};

#endif //TYPHON_TYPE_H