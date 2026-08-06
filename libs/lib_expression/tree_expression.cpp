#include "tree_expression.h"
#include <any>
#include <stdexcept>
#include <sstream>
#include <ostream>
#include "matrix.h"

namespace exprlib {

Matrix perform_tree_expression(const Node* node) {
    if (!node) throw std::runtime_error("Nó nulo.");
    if (node->sym.getType() == Symbol::Type::VALUE) {
        return std::any_cast<Matrix>(node->sym.getValue());
    }

    const auto& op = node->sym.getName();
    Matrix l = perform_tree_expression(node->left.get());
    Matrix r = perform_tree_expression(node->right.get());
    
    if (op == "+") return l + r;
    if (op == "-") return l - r;
    if (op == "*") return l * r;

    throw std::runtime_error("Operador desconhecido: " + op);
}

void printTree(Node* node,std::ostream& out ,std::string indent, bool isLeft) {
    if (!node) return;

    // imprime nó atual
    out << indent;

    if (isLeft) out << "├── ";
    else        out << "└── ";

    out << node->sym.getName() << "\n";

    // aumenta indentação
    indent += isLeft ? "│   " : "    ";

    // recursão
    printTree(node->left.get(), out , indent, true);
    printTree(node->right.get(),out, indent, false);
}


} // namespace exprlib