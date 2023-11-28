#ifndef CODESTATISTICS_H
#define CODESTATISTICS_H

#include <set>
#include <string>
#include <string_view>
#include <algorithm>

class CodeStatistics {
public:
    using StatSize = unsigned int;
    using CSSet = std::set<std::string, std::less<>>; // Uso de std::less<> para simplificar

    CodeStatistics() = default; // Constructor por defecto

    void reset() {
        types = constants = identifiers = cspecs = keywords = operators = conditions = 0;
        typesSet.clear();
        constantsSet.clear();
        identifiersSet.clear();
        cspecsSet.clear();
        keywordsSet.clear();
        operatorsSet.clear();
    }

    void type(std::string_view p)       { types++;       typesSet.insert(std::string(p)); }
    void constant(std::string_view p)   { constants++;   constantsSet.insert(std::string(p)); }
    void identifier(std::string_view p) { identifiers++; identifiersSet.insert(std::string(p)); }
    void cspec(std::string_view p)      { cspecs++;      cspecsSet.insert(std::string(p)); }
    void keyword(std::string_view p)    { keywords++;    keywordsSet.insert(std::string(p)); }
    void op(std::string_view p)         { operators++;   operatorsSet.insert(std::string(p)); }

    void decOp()                        { operators--; }
    void cond()                         { conditions++; }

    StatSize getTypes()       const { return types; }
    StatSize getConstants()   const { return constants; }
    StatSize getIdentifiers() const { return identifiers; }
    StatSize getCspecs()      const { return cspecs; }
    StatSize getKeywords()    const { return keywords; }
    StatSize getOps()         const { return operators; }
    StatSize getConds()       const { return conditions; }

    StatSize getOperands()    const { return types + constants + identifiers; }
    StatSize getOperators()   const { return cspecs + keywords + operators; }

    StatSize getUniqueTypes()       const { return typesSet.size(); }
    StatSize getUniqueConstants()   const { return constantsSet.size(); }
    StatSize getUniqueIdentifiers() const { return identifiersSet.size(); }
    StatSize getUniqueCspecs()      const { return cspecsSet.size(); }
    StatSize getUniqueKeywords()    const { return keywordsSet.size(); }
    StatSize getUniqueOps()         const { return operatorsSet.size(); }

    StatSize getUniqueOperands() const  { return getUniqueTypes() + getUniqueConstants() + getUniqueIdentifiers(); }
    StatSize getUniqueOperators() const { return getUniqueCspecs() + getUniqueKeywords() + getUniqueOps(); }

    CodeStatistics& operator+= (const CodeStatistics& other) {
        types += other.types;
        constants += other.constants;
        identifiers += other.identifiers;
        cspecs += other.cspecs;
        keywords += other.keywords;
        operators += other.operators;
        conditions += other.conditions;

        typesSet.insert(other.typesSet.begin(), other.typesSet.end());
        constantsSet.insert(other.constantsSet.begin(), other.constantsSet.end());
        identifiersSet.insert(other.identifiersSet.begin(), other.identifiersSet.end());
        cspecsSet.insert(other.cspecsSet.begin(), other.cspecsSet.end());
        keywordsSet.insert(other.keywordsSet.begin(), other.keywordsSet.end());
        operatorsSet.insert(other.operatorsSet.begin(), other.operatorsSet.end());

        return *this;
    }

private:
    StatSize types = 0, constants = 0, identifiers = 0, cspecs = 0, keywords = 0, operators = 0, conditions = 0;
    CSSet typesSet, constantsSet, identifiersSet, cspecsSet, keywordsSet, operatorsSet;
};

#endif //CODESTATISTICS_H