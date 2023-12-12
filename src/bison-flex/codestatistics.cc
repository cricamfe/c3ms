#include "codestatistics.hh"
#include "parser.hh"
#include "scanner.hh"

namespace c3ms
{
    CodeStatistics::CodeStatistics()
        : scanner_(std::make_shared<CodeScanner>()),
          parser_(std::make_shared<CodeParser>(*this)),
          error_(0)
    {
        // No need for any other initialization
        // scanner_->set_debug(true);
    }
    int CodeStatistics::parse()
    {
        scanner_->switch_streams(&std::cin, &std::cerr);
        parser_->parse();
        return error_;
    }

    int CodeStatistics::parse(std::istream &iss)
    {
        scanner_->switch_streams(&iss, &std::cerr);
        parser_->parse();
        return error_;
    }

    int CodeStatistics::parse_file(const std::string &path)
    {
        std::ifstream s(path.c_str(), std::ifstream::in);
        scanner_->switch_streams(&s, &std::cerr);
        parser_->parse();
        s.close();
        return error_;
    }

    void CodeStatistics::category(StatsCategory counter, std::string_view p) {
        getCounterReference(counter)++;
        auto& setRef = getCSSetReference(counter);
        auto [it, inserted] = setRef.insert({std::string(p), {1, counter}});
        if (!inserted) {
            it->second.first++;
        }
    }

    CodeStatistics::StatSize CodeStatistics::getCounterValue(StatsCategory set) const {
        switch (set) {
            case StatsCategory::TYPE: return nTypes_;
            case StatsCategory::CONSTANT: return nConstants_;
            case StatsCategory::IDENTIFIER: return nIdentifiers_;
            case StatsCategory::CSPECIFIER: return nCSpecifiers_;
            case StatsCategory::KEYWORD: return nKeywords_;
            case StatsCategory::OPERATOR: return nOperators_;
            case StatsCategory::CONDITION: return nConditions_;
            case StatsCategory::APIKEYWORD: return nAPIKeywords_;
            case StatsCategory::APITYPE: return nAPITypes_;
            case StatsCategory::APICONSTANT: return nAPIConstants_;
            case StatsCategory::APILLKEYWORD: return nAPILLKeywords_;
            case StatsCategory::APILLTYPE: return nAPILLTypes_;
            case StatsCategory::APILLCONSTANT: return nAPILLConstants_;
            case StatsCategory::CUSTOMKEYWORD: return nCustomKeywords_;
            case StatsCategory::CUSTOMTYPE: return nCustomTypes_;
            case StatsCategory::CUSTOMCONSTANT: return nCustomConstants_;
            default: throw std::invalid_argument("Unknown counter type");
        }        
    }

    CodeStatistics::StatSize CodeStatistics::getCSSetSize(StatsCategory set) const {
        switch (set) {
            case StatsCategory::TYPE: return typesSet_.size();
            case StatsCategory::CONSTANT: return constantsSet_.size();
            case StatsCategory::IDENTIFIER: return identifiersSet_.size();
            case StatsCategory::CSPECIFIER: return cSpecifiersSet_.size();
            case StatsCategory::KEYWORD: return keywordsSet_.size();
            case StatsCategory::OPERATOR: return operatorsSet_.size();
            case StatsCategory::CONDITION: return conditionsSet_.size();
            case StatsCategory::APIKEYWORD: return apiKeywordsSet_.size();
            case StatsCategory::APITYPE: return apiTypesSet_.size();
            case StatsCategory::APICONSTANT: return apiConstantsSet_.size();
            case StatsCategory::APILLKEYWORD: return apiLLKeywordsSet_.size();
            case StatsCategory::APILLTYPE: return apiLLTypesSet_.size();
            case StatsCategory::APILLCONSTANT: return apiLLConstantsSet_.size();
            case StatsCategory::CUSTOMKEYWORD: return customKeywordsSet_.size();
            case StatsCategory::CUSTOMTYPE: return customTypesSet_.size();
            case StatsCategory::CUSTOMCONSTANT: return customConstantsSet_.size();
            default: throw std::invalid_argument("Unknown CSSet type");
        }
    }

    CodeStatistics::StatSize CodeStatistics::getUniqueOperands() const  {
        // return getCSSetSize(StatsCategory::CONSTANT) + getCSSetSize(StatsCategory::IDENTIFIER);
        CodeStatistics::StatSize result = 0;
        result = getCSSetSize(StatsCategory::CONSTANT) + getCSSetSize(StatsCategory::IDENTIFIER);
        result += getCSSetSize(StatsCategory::APICONSTANT) + getCSSetSize(StatsCategory::APILLCONSTANT) + getCSSetSize(StatsCategory::CUSTOMCONSTANT);
        result += getCSSetSize(StatsCategory::APITYPE) + getCSSetSize(StatsCategory::APIKEYWORD);
        return result;
    }
    CodeStatistics::StatSize CodeStatistics::getUniqueOperators() const {
        // return getCSSetSize(StatsCategory::CSPECIFIER) + getCSSetSize(StatsCategory::KEYWORD) + getCSSetSize(StatsCategory::OPERATOR) + getCSSetSize(StatsCategory::TYPE);
        CodeStatistics::StatSize result = 0;
        result = getCSSetSize(StatsCategory::CSPECIFIER) + getCSSetSize(StatsCategory::KEYWORD) + getCSSetSize(StatsCategory::OPERATOR) + getCSSetSize(StatsCategory::TYPE);
        result += getCSSetSize(StatsCategory::APILLKEYWORD) + getCSSetSize(StatsCategory::APILLTYPE) + getCSSetSize(StatsCategory::CUSTOMKEYWORD) + getCSSetSize(StatsCategory::CUSTOMTYPE);
        return result;
    }

    CodeStatistics::StatSize CodeStatistics::getOperands() const {
        // return getCounterValue(StatsCategory::CONSTANT) + getCounterValue(StatsCategory::IDENTIFIER);
        CodeStatistics::StatSize result = 0;
        result = getCounterValue(StatsCategory::CONSTANT) + getCounterValue(StatsCategory::IDENTIFIER);
        result += getCounterValue(StatsCategory::APICONSTANT) + getCounterValue(StatsCategory::APILLCONSTANT) + getCounterValue(StatsCategory::CUSTOMCONSTANT);
        result += getCounterValue(StatsCategory::APITYPE) + getCounterValue(StatsCategory::KEYWORD);
        return result;
    }

    CodeStatistics::StatSize CodeStatistics::getOperators() const {
        // return getCounterValue(StatsCategory::CSPECIFIER) + getCounterValue(StatsCategory::KEYWORD) + getCounterValue(StatsCategory::OPERATOR) + getCounterValue(StatsCategory::TYPE);
        CodeStatistics::StatSize result = 0;
        result = getCounterValue(StatsCategory::CSPECIFIER) + getCounterValue(StatsCategory::KEYWORD) + getCounterValue(StatsCategory::OPERATOR) + getCounterValue(StatsCategory::TYPE);
        result += getCounterValue(StatsCategory::APILLKEYWORD) + getCounterValue(StatsCategory::APILLTYPE) + getCounterValue(StatsCategory::CUSTOMKEYWORD) + getCounterValue(StatsCategory::CUSTOMTYPE);
        return result;
    }

    void CodeStatistics::decOperator() { nOperators_--; }
    void CodeStatistics::addCondition() { nConditions_++; }

    void CodeStatistics::reset()
    {
        // Reset error flag
        error_ = 0;
        // Reset all counters
        nTypes_ = 0;
        nConstants_ = 0;
        nIdentifiers_ = 0;
        nCSpecifiers_ = 0;
        nKeywords_ = 0;
        nOperators_ = 0;
        nConditions_ = 0;
        nAPIKeywords_ = 0;
        nAPITypes_ = 0;
        nAPIConstants_ = 0;
        nAPILLKeywords_ = 0;
        nAPILLTypes_ = 0;
        nAPILLConstants_ = 0;
        nCustomKeywords_ = 0;
        nCustomTypes_ = 0;
        nCustomConstants_ = 0;
        // Reset all sets
        typesSet_.clear();
        constantsSet_.clear();
        identifiersSet_.clear();
        cSpecifiersSet_.clear();
        keywordsSet_.clear();
        operatorsSet_.clear();
        conditionsSet_.clear();
        apiKeywordsSet_.clear();
        apiTypesSet_.clear();
        apiConstantsSet_.clear();
        apiLLKeywordsSet_.clear();
        apiLLTypesSet_.clear();
        apiLLConstantsSet_.clear();
        customKeywordsSet_.clear();
        customTypesSet_.clear();
        customConstantsSet_.clear();
    }

    void CodeStatistics::printMetrics(std::ostringstream& result, const CSSet& set, const int nameWidth, const int valueWidth) const {
        for (const auto& element : set) {
            result << std::left << std::setw(nameWidth) << element.first << " " 
                << std::right << std::setw(valueWidth) << element.second.first
                << " (" << toString(element.second.second) << ")\n";
        }
    }

    void CodeStatistics::printHeader(std::ostringstream& result, std::string_view left_header, std::string_view right_header, int nameWidth, int valueWidth, int totalWidth) const {
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


    std::string CodeStatistics::printOperators() const {
        const int nameWidth = 45; // Column width for metric names
        const int valueWidth = 15; // Adjusted column width for metric values
        const int totalWidth = 80; // Total width for both columns
        std::ostringstream result;
        // Adding header
        printHeader(result, "Operators", "Occurrences", nameWidth, valueWidth, totalWidth);
        // Combining all operator sets into a single table
        printMetrics(result, cSpecifiersSet_, nameWidth, valueWidth);
        printMetrics(result, keywordsSet_, nameWidth, valueWidth);
        printMetrics(result, operatorsSet_, nameWidth, valueWidth);
        printMetrics(result, typesSet_, nameWidth, valueWidth);

        return result.str();
    }

    std::string CodeStatistics::printOperands() const {
        const int nameWidth = 45; // Column width for metric names
        const int valueWidth = 15; // Adjusted column width for metric values
        const int totalWidth = 80; // Total width for both columns
        std::ostringstream result;
        // Adding header
        printHeader(result, "Operands", "Occurrences", nameWidth, valueWidth, totalWidth);
        // Combining all operand sets into a single table
        printMetrics(result, constantsSet_, nameWidth, valueWidth);
        printMetrics(result, identifiersSet_, nameWidth, valueWidth);

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
    std::string CodeStatistics::printCustom() const {
        const int nameWidth = 45; // Column width for metric names
        const int valueWidth = 15; // Adjusted column width for metric values
        const int totalWidth = 80; // Total width for both columns
        std::ostringstream result;
        // Adding header
        printHeader(result, "Developed by User", "Occurrences", nameWidth, valueWidth, totalWidth);
        // Combining all operand sets into a single table
        printMetrics(result, customKeywordsSet_, nameWidth, valueWidth);
        printMetrics(result, customTypesSet_, nameWidth, valueWidth);
        printMetrics(result, customConstantsSet_, nameWidth, valueWidth);

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
    std::string CodeStatistics::printAPI() const {
        const int nameWidth = 45; // Column width for metric names
        const int valueWidth = 15; // Adjusted column width for metric values
        const int totalWidth = 80; // Total width for both columns
        std::ostringstream result;
        // Adding header
        printHeader(result, "API", "Occurrences", nameWidth, valueWidth, totalWidth);
        // Combining all operand sets into a single table
        printMetrics(result, apiKeywordsSet_, nameWidth, valueWidth);
        printMetrics(result, apiTypesSet_, nameWidth, valueWidth);
        printMetrics(result, apiConstantsSet_, nameWidth, valueWidth);

        return result.str();
    }

    /**
     * @brief Prints a formatted table of low-level API function calls and their occurrences.
     * 
     * @details This function prints a formatted table of low-level API function calls and their occurrences.
     * Each row of the table contains the name of the low-level API function call and its occurrence count. The table is formatted with a specified column width for the low-level API function call names and the occurrence counts.
     * 
     * @return A string representing the formatted table.
     */
    std::string CodeStatistics::printAPILowLevel() const {
        const int nameWidth = 45; // Column width for metric names
        const int valueWidth = 15; // Adjusted column width for metric values
        const int totalWidth = 80; // Total width for both columns
        std::ostringstream result;
        // Adding header
        printHeader(result, "API (Low Level)", "Occurrences", nameWidth, valueWidth, totalWidth);
        // Combining all operand sets into a single table
        printMetrics(result, apiLLKeywordsSet_, nameWidth, valueWidth);
        printMetrics(result, apiLLTypesSet_, nameWidth, valueWidth);
        printMetrics(result, apiLLConstantsSet_, nameWidth, valueWidth);

        return result.str();
    }

    CodeStatistics& CodeStatistics::operator+=(const CodeStatistics& rhs) {
        nTypes_ += rhs.nTypes_;
        nConstants_ += rhs.nConstants_;
        nIdentifiers_ += rhs.nIdentifiers_;
        nCSpecifiers_ += rhs.nCSpecifiers_;
        nKeywords_ += rhs.nKeywords_;
        nOperators_ += rhs.nOperators_;
        nConditions_ += rhs.nConditions_;
        nAPIKeywords_ += rhs.nAPIKeywords_;
        nAPITypes_ += rhs.nAPITypes_;
        nAPIConstants_ += rhs.nAPIConstants_;
        nAPILLKeywords_ += rhs.nAPILLKeywords_;
        nAPILLTypes_ += rhs.nAPILLTypes_;
        nAPILLConstants_ += rhs.nAPILLConstants_;
        nCustomKeywords_ += rhs.nCustomKeywords_;
        nCustomTypes_ += rhs.nCustomTypes_;
        nCustomConstants_ += rhs.nCustomConstants_;

        auto combineCSSets = [](CSSet& lhsSet, const CSSet& rhsSet) {
            for (const auto& element : rhsSet) {
                auto [it, inserted] = lhsSet.insert({element.first, {element.second.first, element.second.second}});
                if (!inserted) {
                    it->second.first += element.second.first;
                }
            }
        };

        combineCSSets(typesSet_, rhs.typesSet_);
        combineCSSets(constantsSet_, rhs.constantsSet_);
        combineCSSets(identifiersSet_, rhs.identifiersSet_);
        combineCSSets(cSpecifiersSet_, rhs.cSpecifiersSet_);
        combineCSSets(keywordsSet_, rhs.keywordsSet_);
        combineCSSets(operatorsSet_, rhs.operatorsSet_);
        combineCSSets(conditionsSet_, rhs.conditionsSet_);
        combineCSSets(apiKeywordsSet_, rhs.apiKeywordsSet_);
        combineCSSets(apiTypesSet_, rhs.apiTypesSet_);
        combineCSSets(apiConstantsSet_, rhs.apiConstantsSet_);
        combineCSSets(apiLLKeywordsSet_, rhs.apiLLKeywordsSet_);
        combineCSSets(apiLLTypesSet_, rhs.apiLLTypesSet_);
        combineCSSets(apiLLConstantsSet_, rhs.apiLLConstantsSet_);
        combineCSSets(customKeywordsSet_, rhs.customKeywordsSet_);
        combineCSSets(customTypesSet_, rhs.customTypesSet_);
        combineCSSets(customConstantsSet_, rhs.customConstantsSet_);

        return *this;
    }

    CodeStatistics operator+(CodeStatistics lhs, const CodeStatistics& rhs) {
        lhs += rhs;
        return lhs;
    }

    CodeStatistics::StatSize& CodeStatistics::getCounterReference(StatsCategory counter) {
        switch (counter) {
            case StatsCategory::TYPE: return nTypes_;
            case StatsCategory::CONSTANT: return nConstants_;
            case StatsCategory::IDENTIFIER: return nIdentifiers_;
            case StatsCategory::CSPECIFIER: return nCSpecifiers_;
            case StatsCategory::KEYWORD: return nKeywords_;
            case StatsCategory::OPERATOR: return nOperators_;
            case StatsCategory::CONDITION: return nConditions_;
            case StatsCategory::APIKEYWORD: return nAPIKeywords_;
            case StatsCategory::APITYPE: return nAPITypes_;
            case StatsCategory::APICONSTANT: return nAPIConstants_;
            case StatsCategory::APILLKEYWORD: return nAPILLKeywords_;
            case StatsCategory::APILLTYPE: return nAPILLTypes_;
            case StatsCategory::APILLCONSTANT: return nAPILLConstants_;
            case StatsCategory::CUSTOMKEYWORD: return nCustomKeywords_;
            case StatsCategory::CUSTOMTYPE: return nCustomTypes_;
            case StatsCategory::CUSTOMCONSTANT: return nCustomConstants_;
            default: throw std::invalid_argument("Unknown counter type");
        }
    }

    CodeStatistics::CSSet& CodeStatistics::getCSSetReference(StatsCategory set) {
        switch (set) {
            case StatsCategory::TYPE: return typesSet_;
            case StatsCategory::CONSTANT: return constantsSet_;
            case StatsCategory::IDENTIFIER: return identifiersSet_;
            case StatsCategory::CSPECIFIER: return cSpecifiersSet_;
            case StatsCategory::KEYWORD: return keywordsSet_;
            case StatsCategory::OPERATOR: return operatorsSet_;
            case StatsCategory::CONDITION: return conditionsSet_;
            case StatsCategory::APIKEYWORD: return apiKeywordsSet_;
            case StatsCategory::APITYPE: return apiTypesSet_;
            case StatsCategory::APICONSTANT: return apiConstantsSet_;
            case StatsCategory::APILLKEYWORD: return apiLLKeywordsSet_;
            case StatsCategory::APILLTYPE: return apiLLTypesSet_;
            case StatsCategory::APILLCONSTANT: return apiLLConstantsSet_;
            case StatsCategory::CUSTOMKEYWORD: return customKeywordsSet_;
            case StatsCategory::CUSTOMTYPE: return customTypesSet_;
            case StatsCategory::CUSTOMCONSTANT: return customConstantsSet_;
            default: throw std::invalid_argument("Unknown CSSet type");
        }
    }

    std::string CodeStatistics::toString(StatsCategory category) const {
        switch (category) {
            case StatsCategory::TYPE: return "Type";
            case StatsCategory::CONSTANT: return "Constant";
            case StatsCategory::IDENTIFIER: return "Identifier";
            case StatsCategory::CSPECIFIER: return "C Specifier";
            case StatsCategory::KEYWORD: return "Keyword";
            case StatsCategory::OPERATOR: return "Operator";
            case StatsCategory::CONDITION: return "Condition";
            case StatsCategory::APIKEYWORD: return "API Keyword";
            case StatsCategory::APITYPE: return "API Type";
            case StatsCategory::APICONSTANT: return "API Constant";
            case StatsCategory::APILLKEYWORD: return "API Low Level Keyword";
            case StatsCategory::APILLTYPE: return "API Low Level Type";
            case StatsCategory::APILLCONSTANT: return "API Low Level Constant";
            case StatsCategory::CUSTOMKEYWORD: return "Custom Keyword";
            case StatsCategory::CUSTOMTYPE: return "Custom Type";
            case StatsCategory::CUSTOMCONSTANT: return "Custom Constant";
            default: return "Unknown";
        }
    }
}
