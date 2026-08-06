#ifndef TREE_EXPRESSION_H_INCLUDED
#define TREE_EXPRESSION_H_INCLUDED

#pragma once
#include "symbol_node.h"
#include "matrix.h"
#include <ostream>

namespace exprlib {

/**
 * @brief Function to perform a tree expression.
 * @details Permorm the operations organized in a tree expression to reach a result value.
 * @param Node Top node of a expression.
 * @return Matrix object with the result of expression.
 */
Matrix perform_tree_expression(const Node* node);

/**
 * @brief Function to criate a grafic of expression tree.
 * @param node Top node of tree or subtree
 * @param out A output stream to store a grafic tree expression organization.
 * @param indent String indent do organize connections between nodes.
 * @param isLeft Select the side of a node
 */
void printTree(Node* node,std::ostream& out, std::string indent = "", bool isLeft = true);
    
} // namespace exprlib

#endif // TREE_EXPRESSION_H_INCLUDED