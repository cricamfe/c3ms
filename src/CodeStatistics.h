#ifndef CODESTATISTICS_H
#define CODESTATISTICS_H

#include <unordered_map>
#include <string>
#include <string_view>
#include <algorithm>
#include <sstream>
#include <iomanip>

/**
 * @class CodeStatistics
 * 
 * @brief A class that collects and provides statistics about a piece of code.
 * 
 * @details This class collects statistics about a piece of code, including the number of occurrences of various types of tokens (types, constants, identifiers, cspecs, keywords, operators, conditions).
 * The class provides methods to increment the count of each type of token, and to get the total count of each type of token, as well as the total count of operands and operators.
 * The class also provides methods to get the count of unique occurrences of each type of token, and the total count of unique operands and operators.
 */
class CodeStatistics {
public:
    using StatSize = unsigned int;
    using CSSet = std::unordered_map<std::string, int>; // Uso de std::unordered_map para contar ocurrencias

    /**
     * @brief Default constructor.
     */
    CodeStatistics() = default; // Constructor por defecto

    /**
     * @brief Resets all the statistics to zero.
     */
    void reset() {
        types = constants = identifiers = cspecs = keywords = operators = conditions = customFunctionCalls = apiFunctionCalls = 0;
        typesSet.clear();
        constantsSet.clear();
        identifiersSet.clear();
        cspecsSet.clear();
        keywordsSet.clear();
        operatorsSet.clear();
        customFunctionCallsSet.clear();
        apiFunctionCallsSet.clear();
    }

    /**
     * @brief Increments the count of a type token.
     * 
     * @param p The type token.
     */
    void type(std::string_view p)       { types++;       typesSet[std::string(p)]++; }
    /**
     * @brief Increments the count of a constant token.
     * 
     * @param p The constant token.
     */
    void constant(std::string_view p)   { constants++;   constantsSet[std::string(p)]++; }
    /**
     * @brief Increments the count of an identifier token.
     * 
     * @param p The identifier token.
     */
    void identifier(std::string_view p) { identifiers++; identifiersSet[std::string(p)]++; }
    /**
     * @brief Increments the count of a cspec token.
     * 
     * @param p The cspec token.
     */
    void cspec(std::string_view p)      { cspecs++;      cspecsSet[std::string(p)]++; }
    /**
     * @brief Increments the count of a keyword token.
     * 
     * @param p The keyword token.
     */
    void keyword(std::string_view p)    { keywords++;    keywordsSet[std::string(p)]++; }
    /**
     * @brief Increments the count of an operator token.
     * 
     * @param p The operator token.
     */
    void op(std::string_view p)         { operators++;   operatorsSet[std::string(p)]++; }
    /**
     * @brief Increments the count of a customFunctionCall token.
     * 
     * @param p The condition token.
     */
    void customFunctionCall(std::string_view p) { customFunctionCalls++; customFunctionCallsSet[std::string(p)]++; }
    /**
     * @brief Increments the count of an apiFunctionCall token.
     * 
     * @param p The condition token.
     */
    void apiFunctionCall(std::string_view p) { apiFunctionCalls++; apiFunctionCallsSet[std::string(p)]++; }

    /**
     * @brief Decrements the count of an operator token.
     */
    void decOp()                        { operators--; }
    /**
     * @brief Increments the count of a condition token.
     */
    void cond()                         { conditions++; }

    /**
     * @brief Returns the total count of type tokens.
     * 
     * @return The total count of type tokens.
     */
    StatSize getTypes()       const { return types; }
    /**
     * @brief Returns the total count of constant tokens.
     * 
     * @return The total count of constant tokens.
     */
    StatSize getConstants()   const { return constants; }
    /**
     * @brief Returns the total count of identifier tokens.
     * 
     * @return The total count of identifier tokens.
     */
    StatSize getIdentifiers() const { return identifiers; }
    /**
     * @brief Returns the total count of cspec tokens.
     * 
     * @return The total count of cspec tokens.
     */
    StatSize getCspecs()      const { return cspecs; }
    /**
     * @brief Returns the total count of keyword tokens.
     * 
     * @return The total count of keyword tokens.
     */
    StatSize getKeywords()    const { return keywords; }
    /**
     * @brief Returns the total count of operator tokens.
     * 
     * @return The total count of operator tokens.
     */
    StatSize getOps()         const { return operators; }
    /**
     * @brief Returns the total count of condition tokens.
     * 
     * @return The total count of condition tokens.
     */
    StatSize getConds()       const { return conditions; }
    /**
     * @brief Returns the total count of customFunctionCalls tokens.
     * 
     * @return The total count of customFunctionCalls tokens.
     */
    StatSize getCustomFunctionCalls() const { return customFunctionCalls; }
    /**
     * @brief Returns the total count of apiFunctionCalls tokens.
     * 
     * @return The total count of apiFunctionCalls tokens.
     */
    StatSize getApiFunctionCalls() const { return apiFunctionCalls; }


    /**
     * @brief Returns the total count of operand tokens (constants + identifiers).
     * 
     * @return The total count of operand tokens.
     */
    StatSize getOperands()    const { return constants + identifiers; }
    /**
     * @brief Returns the total count of operator tokens (cspecs + keywords + operators + types).
     * 
     * @return The total count of operator tokens.
     */
    StatSize getOperators()   const { return cspecs + keywords + operators + types; }

    /**
     * @brief Returns the total count of unique type tokens.
     * 
     * @return The total count of unique type tokens.
     */
    StatSize getUniqueTypes()       const { return typesSet.size(); }
    /**
     * @brief Returns the total count of unique constant tokens.
     * 
     * @return The total count of unique constant tokens.
     */
    StatSize getUniqueConstants()   const { return constantsSet.size(); }
    /**
     * @brief Returns the total count of unique identifier tokens.
     * 
     * @return The total count of unique identifier tokens.
     */
    StatSize getUniqueIdentifiers() const { return identifiersSet.size(); }
    /**
     * @brief Returns the total count of unique cspec tokens.
     * 
     * @return The total count of unique cspec tokens.
     */
    StatSize getUniqueCspecs()      const { return cspecsSet.size(); }
    /**
     * @brief Returns the total count of unique keyword tokens.
     * 
     * @return The total count of unique keyword tokens.
     */
    StatSize getUniqueKeywords()    const { return keywordsSet.size(); }
    /**
     * @brief Returns the total count of unique operator tokens.
     * 
     * @return The total count of unique operator tokens.
     */
    StatSize getUniqueOps()         const { return operatorsSet.size(); }
    /**
     * @brief Returns the total count of unique customFunctionsCalls tokens.
     * 
     * @return The total count of unique customFunctionsCalls tokens.
     */
    StatSize getUniqueCustomFunctionCalls() const { return customFunctionCallsSet.size(); }
    /**
     * @brief Returns the total count of unique apiFunctionCalls tokens.
     * 
     * @return The total count of unique apiFunctionCalls tokens.
     */
    StatSize getUniqueApiFunctionCalls() const { return apiFunctionCallsSet.size(); }

    /**
     * @brief Returns the total count of unique operand tokens (unique constants + unique identifiers).
     * 
     * @return The total count of unique operand tokens.
     */
    StatSize getUniqueOperands() const  { return getUniqueConstants() + getUniqueIdentifiers(); }
    /**
     * @brief Returns the total count of unique operator tokens (unique cspecs + unique keywords + unique operators + unique types).
     * 
     * @return The total count of unique operator tokens.
     */
    StatSize getUniqueOperators() const { return getUniqueCspecs() + getUniqueKeywords() + getUniqueOps() + getUniqueTypes(); }

    /**
     * @brief Prints a formatted table of metrics.
     * 
     * @details This function prints a formatted table of metrics. Each row of the table contains the name of the metric and its value. The table is formatted with a specified column width for the metric names and the values.
     * 
     * @param result The output stream to which the table is printed.
     * @param set The set of metrics to be printed.
     * @param nameWidth The width of the column for the metric names.
     * @param valueWidth The width of the column for the values.
     */
    void printMetrics(std::ostringstream& result, const CSSet& set, const int nameWidth, const int valueWidth) const {
        for (const auto& element : set) {
            result << std::left << std::setw(nameWidth) << element.first << " " 
                   << std::right << std::setw(valueWidth) << element.second << '\n';
        }
    }

    /**
     * @brief Prints a formatted header for a table.
     * 
     * @details This function prints a formatted header for a table, with a left-aligned title on the left side of the table
     * and a centered title on the right side of the table. The table is separated by a line of dashes.
     * 
     * @param result The output stream to which the header is printed.
     * @param left_header The title to be printed on the left side of the table.
     * @param right_header The title to be printed on the right side of the table.
     * @param nameWidth The width of the left side of the table.
     * @param valueWidth The width of the right side of the table.
     * @param totalWidth The total width of the table.
     */
    void printHeader(std::ostringstream& result, const std::string& left_header, const std::string& right_header, const int& nameWidth, const int& valueWidth, const int& totalWidth) const {
        result << std::string(totalWidth, '-') << "\n"; // Print a line of dashes
        // Print the left header
        result << std::left << std::setw(nameWidth) << left_header;
        // Calculate the remaining width for the right header
        int rightHeaderWidth = totalWidth - nameWidth;
        // Align the right header within the remaining width
        int padding = (rightHeaderWidth - right_header.length()) / 2;
        result << std::string(padding-1, ' ') << right_header << "\n";
        result << std::string(totalWidth, '-') << "\n"; // Print another line of dashes
    }

    /**
     * @brief Prints a formatted table of operators and their occurrences.
     * 
     * @details This function prints a formatted table of operators and their occurrences. The table includes cspecs, keywords, operators, and types.
     * Each row of the table contains the name of the operator and its occurrence count. The table is formatted with a specified column width for the operator names and the occurrence counts.
     * 
     * @return A string representing the formatted table.
     */
    std::string printOperators() const {
        const int nameWidth = 45; // Column width for metric names
        const int valueWidth = 15; // Adjusted column width for metric values
        const int totalWidth = 80; // Total width for both columns
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

    /**
     * @brief Prints a formatted table of operands and their occurrences.
     * 
     * @details This function prints a formatted table of operands and their occurrences. The table includes constants and identifiers.
     * Each row of the table contains the name of the operand and its occurrence count. The table is formatted with a specified column width for the operand names and the occurrence counts.
     * 
     * @return A string representing the formatted table.
     */
    std::string printOperands() const {
        const int nameWidth = 45; // Column width for metric names
        const int valueWidth = 15; // Adjusted column width for metric values
        const int totalWidth = 80; // Total width for both columns
        std::ostringstream result;
        // Adding header
        printHeader(result, "Operands", "Occurrences", nameWidth, valueWidth, totalWidth);
        // Combining all operand sets into a single table
        printMetrics(result, constantsSet, nameWidth, valueWidth);
        printMetrics(result, identifiersSet, nameWidth, valueWidth);

        return result.str();
    }

    /**
     * @brief Prints a formatted table of custom function calls and their occurrences.
     * 
     * @details This function prints a formatted table of custom function calls and their occurrences.
     * Each row of the table contains the name of the custom function call and its occurrence count. The table is formatted with a specified column width for the custom function call names and the occurrence counts.
     * 
     * @return A string representing the formatted table.
     */
    std::string printCustomFunctionCalls() const {
        const int nameWidth = 45; // Column width for metric names
        const int valueWidth = 15; // Adjusted column width for metric values
        const int totalWidth = 80; // Total width for both columns
        std::ostringstream result;
        // Adding header
        printHeader(result, "Custom Function Calls", "Occurrences", nameWidth, valueWidth, totalWidth);
        // Combining all operand sets into a single table
        printMetrics(result, customFunctionCallsSet, nameWidth, valueWidth);

        return result.str();
    }

    /**
     * @brief Prints a formatted table of API function calls and their occurrences.
     * 
     * @details This function prints a formatted table of API function calls and their occurrences.
     * Each row of the table contains the name of the API function call and its occurrence count. The table is formatted with a specified column width for the API function call names and the occurrence counts.
     * 
     * @return A string representing the formatted table.
     */
    std::string printApiFunctionCalls() const {
        const int nameWidth = 45; // Column width for metric names
        const int valueWidth = 15; // Adjusted column width for metric values
        const int totalWidth = 80; // Total width for both columns
        std::ostringstream result;
        // Adding header
        printHeader(result, "API Function Calls", "Occurrences", nameWidth, valueWidth, totalWidth);
        // Combining all operand sets into a single table
        printMetrics(result, apiFunctionCallsSet, nameWidth, valueWidth);

        return result.str();
    }

    /**
     * @brief Overloads the += operator to combine the statistics of two CodeStatistics objects.
     * 
     * @details This operator overload allows the statistics of two CodeStatistics objects to be combined. The counts of each type of token are added together, and the sets of unique tokens are merged.
     * 
     * @param other The other CodeStatistics object whose statistics are to be added to this object.
     * @return A reference to this object, after the statistics have been combined.
     */
    CodeStatistics& operator+= (const CodeStatistics& other) {
        types += other.types;
        constants += other.constants;
        identifiers += other.identifiers;
        cspecs += other.cspecs;
        keywords += other.keywords;
        operators += other.operators;
        conditions += other.conditions;
        customFunctionCalls += other.customFunctionCalls;
        apiFunctionCalls += other.apiFunctionCalls;

        typesSet.insert(other.typesSet.begin(), other.typesSet.end());
        constantsSet.insert(other.constantsSet.begin(), other.constantsSet.end());
        identifiersSet.insert(other.identifiersSet.begin(), other.identifiersSet.end());
        cspecsSet.insert(other.cspecsSet.begin(), other.cspecsSet.end());
        keywordsSet.insert(other.keywordsSet.begin(), other.keywordsSet.end());
        operatorsSet.insert(other.operatorsSet.begin(), other.operatorsSet.end());
        customFunctionCallsSet.insert(other.customFunctionCallsSet.begin(), other.customFunctionCallsSet.end());
        apiFunctionCallsSet.insert(other.apiFunctionCallsSet.begin(), other.apiFunctionCallsSet.end());

        return *this;
    }

private:
    /**
     * @brief The count of each type of token.
     */
    StatSize types = 0, constants = 0, identifiers = 0, cspecs = 0, keywords = 0, operators = 0, conditions = 0;
    StatSize customFunctionCalls = 0;
    StatSize apiFunctionCalls = 0;
    /**
     * @brief The sets of unique tokens of each type.
     */
    CSSet typesSet, constantsSet, identifiersSet, cspecsSet, keywordsSet, operatorsSet;
    CSSet customFunctionCallsSet;
    CSSet apiFunctionCallsSet;
};

#endif //CODESTATISTICS_H