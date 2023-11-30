#ifndef CODESTATISTICS_H
#define CODESTATISTICS_H

#include <unordered_map>
#include <string>
#include <string_view>
#include <algorithm>
#include <sstream>
#include <iomanip>

class CodeStatistics {
public:
    using StatSize = unsigned int;
    using CSSet = std::unordered_map<std::string, int>; // Uso de std::unordered_map para contar ocurrencias

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

    void type(std::string_view p)       { types++;       typesSet[std::string(p)]++; }
    void constant(std::string_view p)   { constants++;   constantsSet[std::string(p)]++; }
    void identifier(std::string_view p) { identifiers++; identifiersSet[std::string(p)]++; }
    void cspec(std::string_view p)      { cspecs++;      cspecsSet[std::string(p)]++; }
    void keyword(std::string_view p)    { keywords++;    keywordsSet[std::string(p)]++; }
    void op(std::string_view p)         { operators++;   operatorsSet[std::string(p)]++; }

    void decOp()                        { operators--; }
    void cond()                         { conditions++; }

    StatSize getTypes()       const { return types; }
    StatSize getConstants()   const { return constants; }
    StatSize getIdentifiers() const { return identifiers; }
    StatSize getCspecs()      const { return cspecs; }
    StatSize getKeywords()    const { return keywords; }
    StatSize getOps()         const { return operators; }
    StatSize getConds()       const { return conditions; }

    StatSize getOperands()    const { return constants + identifiers; }
    StatSize getOperators()   const { return cspecs + keywords + operators + types; }

    StatSize getUniqueTypes()       const { return typesSet.size(); }
    StatSize getUniqueConstants()   const { return constantsSet.size(); }
    StatSize getUniqueIdentifiers() const { return identifiersSet.size(); }
    StatSize getUniqueCspecs()      const { return cspecsSet.size(); }
    StatSize getUniqueKeywords()    const { return keywordsSet.size(); }
    StatSize getUniqueOps()         const { return operatorsSet.size(); }

    StatSize getUniqueOperands() const  { return getUniqueConstants() + getUniqueIdentifiers(); }
    StatSize getUniqueOperators() const { return getUniqueCspecs() + getUniqueKeywords() + getUniqueOps() + getUniqueTypes(); }

    // Función para imprimir un conjunto de métricas
    void printMetrics(std::ostringstream& result, const CSSet& set, const int nameWidth, const int valueWidth) const {
        for (const auto& element : set) {
            result << std::left << std::setw(nameWidth) << element.first << " " 
                   << std::right << std::setw(valueWidth) << element.second << '\n';
        }
    }

    void printHeader(std::ostringstream& result, const std::string& left_header, const std::string& right_header, const int& nameWidth, const int& valueWidth, const int& totalWidth) const {
        result << std::string(totalWidth, '-') << "\n";
        result << std::left << std::setw(nameWidth) << left_header;
        result << std::right << std::setw(valueWidth + (nameWidth - valueWidth) / 2) << right_header << "\n";
        result << std::string(totalWidth, '-') << "\n";
    }

    std::string printOperators() const {
        const int nameWidth = 40; // Column width for metric names
        const int valueWidth = 15; // Adjusted column width for metric values
        const int totalWidth = 75; // Total width for both columns
        std::ostringstream result;

        // Adding header
        printHeader(result, "Operators", "Occurrences", nameWidth, valueWidth, totalWidth);

        // Combining all operator sets into a single table
        printMetrics(result, cspecsSet, nameWidth, valueWidth);
        printMetrics(result, keywordsSet, nameWidth, valueWidth);
        printMetrics(result, operatorsSet, nameWidth, valueWidth);
        printMetrics(result, typesSet, nameWidth, valueWidth);

        return result.str();
    }

    std::string printOperands() const {
        const int nameWidth = 40; // Column width for metric names
        const int valueWidth = 15; // Adjusted column width for metric values
        const int totalWidth = 75; // Total width for both columns
        std::ostringstream result;

        // Adding header
        printHeader(result, "Operands", "Occurrences", nameWidth, valueWidth, totalWidth);
        
        // Combining all operand sets into a single table
        printMetrics(result, constantsSet, nameWidth, valueWidth);
        printMetrics(result, identifiersSet, nameWidth, valueWidth);

        return result.str();
    }

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