#ifndef TYPHON_LEXER_H
#define TYPHON_LEXER_H
#include <string>
#include <vector>
#include "Token.h"


class Lexer {
public:
    Lexer();
    ~Lexer();

    std::vector<Token> tokenize(const std::vector<std::string> &rawTokens);
    std::vector<Token> tokenize(std::string &file);
    static std::vector<std::string> tokenize_raw(std::string &file);
};

#endif //TYPHON_LEXER_H