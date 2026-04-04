//
// Created by justinw on 30/03/2026.
//

#ifndef TYPHON_UTILS_H
#define TYPHON_UTILS_H
#include "parser/ASTNode.h"

bool eq(ASTNode* n1, ASTNode* n2);
bool eq_tree(std::vector<std::unique_ptr<ASTNode>>& tree1, std::vector<std::unique_ptr<ASTNode>>& tree2);

void print_tree(std::vector<std::unique_ptr<ASTNode>>& tree, int indent=0);
#endif //TYPHON_UTILS_H