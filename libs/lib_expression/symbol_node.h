#ifndef SIMBOL_NODE_H_INCLUDED
#define SIMBOL_NODE_H_INCLUDED

#pragma once
#include <memory>
#include "symbol.h"
#include <iostream>

namespace exprlib {

/**
 * @brief Struct of a Symbol node.
 * @details This node can be used to organize a list of symbols 
 * in a specified order to data analisys.
 */
struct Node {
    /**
     * @brief Symbol variable.
     * @details Store informations about a symbol.
     */
    Symbol sym;
    /**
     * @brief left - Pointer to the left node's children
     */
    std::unique_ptr<Node> left;
    /**
     * @brief left - Pointer to the right node's children
     */
    std::unique_ptr<Node> right;
    /**
     * @brief Constructor of the struct
     * @param s Symbol object to store
     */
    explicit Node(Symbol s) : sym(std::move(s)) {}
};

/**
 * @brief Alias to node pointer
 */
using NodePtr = std::unique_ptr<Node>;

/**
 * @brief Function to create a operand node.
 * @param op Name of a operator.
 * @param left Node that should be placed in left side.
 * @param right Node that should be placed in right side.
 * @return New node with his children.
 */
inline NodePtr makeOp(std::string op, NodePtr left, NodePtr right) {
    auto n = std::make_unique<Node>(Symbol(std::move(op)));
    n->left = std::move(left);
    n->right = std::move(right);
    return n;
}


} // namespace exprlib

#endif // SIMBOL_NODE_H_INCLUDED