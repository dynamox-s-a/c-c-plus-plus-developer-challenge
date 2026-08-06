#pragma once
#include <string>
#include "symbol_node.h"

namespace exprlib {





/** 
 * @brief Class for convert a string expression in a node tree of operations.
 * @details This methods are use to analyse the math expression. Analyse the 
 * string expression to identify, classify and organize tokens to construct a
 * node operation tree for execute the calc. 
 */
class Parser {
public:

    struct ValidationResult {
        bool ok;
        std::string error;
        int tokenIndex; // índice do token onde deu erro (0-based)
    };

    // parse("3 + 5 * 2") expression
    /**
     * @brief method to construct the operation tree.
     * @param String of math expression.
     * @return Top tree node of math operation.
     */
    static NodePtr parseInfix(const std::string& expr);
    /**
     * @brief Method to validate a expression.
     * @param expr String with expression to be analyse.
     * @return Object with informations about analyses. 
     */
    static ValidationResult validateExpressionTokens(const std::string& expr);

private:
    /**
     * @brief Method to get the level procedence of a operator.
     * @param op String of operator
     * @return level of operation procedence: 2 (*) greater than level 1 (+,-) of procedence in a expression.
     */ 
    static int precedence(const std::string& op);
    /**
     * @brief Method to check if a token is a expression value.
     * @param t String of token.
     * @return true - if is a value of expression.
     */
    static bool isNumberToken(const std::string& t);
    /**
     * @brief Method to check if a token is a expression operator.
     * @param t String of token. 
     * @return true - if is a expression operator.
     */
    static bool isOperator(const std::string& t);
    /**
     * @brief Method to check if the token is a parent open
     * @param t String with token to be analyse.
     * @return true if it's the parent open, otherwise false.
     */
    static bool isParenOpen(const std::string& t);
    /**
     * @brief Method to check if the token is a parent close
     * @param t String with token to be analyse.
     * @return true if it's the parent close, otherwise false.
     */
    static bool isParenClose(const std::string& t);
};

} // namespace exprlib