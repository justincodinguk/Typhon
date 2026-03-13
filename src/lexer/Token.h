#ifndef TYPHON_TOKEN_H
#define TYPHON_TOKEN_H
#include <string>
#include <variant>

#endif //TYPHON_TOKEN_H

enum class TokenType {
    // Symbols
    LBracket, RBracket, LBrace, RBrace, Comma, Semicolon, Colon, Arrow, Newline, EndOfFile, Dot,
    LParen, RParen,
    // Binary Operator
    Plus, Minus, Multiply, Divide, Modulo, Power, FloorDivide, Assign, Equal, NotEqual, LessThan,
    GreaterThan, PlusAssign, MinusAssign, MultiplyAssign, DivideAssign, ModuloAssign, PowerAssign,
    FloorDivideAssign, LessThanOrEqual, GreaterThanOrEqual, DoubleAmpersand, DoublePipe,
    // Unary Operator
    Increment, Decrement, UnaryMinus, UnaryPlus, Bang,
    // Keywords
    If, Else, Elif, While, For, Def, Class, Return, Ext, Import, As, Try, Catch, Finally, With,
    Break, Continue, And, Or, Not, In, Is, True, False, Nonlocal, Del, Assert, Async, Await, Public, Internal, Private,
    // Constants
    Identifier, Literal, IntLiteral, StringLiteral, FloatLiteral

};

struct Token {
    TokenType type;
    std::variant<std::monostate, std::string, double> value;
    int line, column;

    std::string toString();

    static Token monostate(TokenType type, int line, int column);
};