cmake_minimum_required(VERSION 4.1)
project(pyc__)

set(CMAKE_CXX_STANDARD 23)

add_executable(pyc__ main.cpp
        lexer/Token.cpp
        lexer/Lexer.cpp
        lexer/Lexer.h
        lexer/Token.h
        parser/Parser.cpp
        parser/Parser.h
        parser/ASTNode.cpp
        parser/ASTNode.h
        parser/Visibility.h)
