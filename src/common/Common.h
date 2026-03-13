//
// Created by justinw on 10/03/2026.
//

#ifndef TYPHON_COMMON_H
#define TYPHON_COMMON_H

enum Precedence {
    NONE,
    ASSIGNMENT,
    OR,
    AND,
    EQUALITY,
    COMPARISON,
    TERM,
    FACTOR,
    EXPONENT,
    UNARY,
    CALL,
    PRIMARY
};

enum class Visibility {
    PUBLIC, PRIVATE, INTERNAL
};

#endif //TYPHON_COMMON_H