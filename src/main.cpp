#include <iostream>
#include <fstream>

#include "analyzer/SemanticAnalyzer.h"
#include "emitter/PythonEmitter.h"
#include "lexer/Token.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"

int main(int argc, char** argv) {
    if (argc<2) {
        std::cout << "\033[1;31mERROR: \033[0m" << "No input files\nExiting now\n" << std::endl;;
        return 1;
    }

    for (int i=1;i<argc;++i) {
        if (std::ifstream file(argv[i]); file.is_open()) {
            std::string contents((std::istreambuf_iterator(file)), std::istreambuf_iterator<char>());

            Lexer lexer;
            auto tokens = lexer.tokenize(contents);

            Parser parser(tokens);
            auto ast = parser.parse();

            SemanticAnalyzer analyzer;
            ast = analyzer.analyze(std::move(ast));

            PythonEmitter pyEmitter(std::move(ast));
            pyEmitter.writeToFile(std::string(argv[i])+".py");
        } else {
            std::cerr << "Could not open file" << argv[i] << "\n";
        }
    }
}
