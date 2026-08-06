#ifndef SYMBOL_H_INCLUDED
#define SYMBOL_H_INCLUDED

#include <any>
#include <string>

namespace exprlib {

/**
 * @brief Class to represent a symbol in a expression.
 * @param type_ Symbol type
 * @param name String to represent the symbol in a expression
 * @param value Informaton that represent a value in expression like matrix.
 */
class Symbol {
private:
    int type;
    std::string name;
    std::any value;
public:
    /// @brief Types of symbols 
    /**
     * @brief Defined types of Symbol to use in type variable.
     */
    enum Type{ EMPTY= 0, VALUE = 1, OPERATION = 2 };
    /**
     * @brief Class constructor.
     */
    Symbol(){};
    /**
     * @brief Class constructor
     * @param pName String to represent the simbol in the expression
     */
    Symbol(std::string pName): name(pName) {}
    /**
     * @brief Destructor
     */
    ~Symbol() = default;
    /**
     * @brief Config value of symbol
     * @param pValue Type value
     */
    void setValue(std::any pValue){value = std::move(pValue);}
    /**
     * @brief Read Value of symbol
     * @return value stored without a type
     */
    std::any getValue()const {return value;} 
    /**
     * @brief Read type of symbol.
     * @details Read a Identifier by a know type symbol.
     */
    int getType()const{return type;}
    /**
     * @brief Config a type
     * @details Store a Type ID by a know type symbol.
     */
    void setType(int ptype){type = ptype;}
    /**
     * @brief Config the symbol's name.
     * @details Symbol's name is extracted from expression.
     * @param pName String of symbol's name.
     */
    void setName(std::string pName){name=pName;}
    /**
     * @brief Read the symbol's name.
     * @return A string of symbol's name.
     */
    std::string getName()const{return name;}
};


} // namespace exprlib

#endif // SYMBOL_H_INCLUDED