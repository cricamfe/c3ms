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
        auto [it, inserted] = setRef.insert({std::string(p), 1});
        if (!inserted) {
            it->second++;
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
            case StatsCategory::APIFUNCTION: return nAPIFunctions_;
            case StatsCategory::CUSTOMFUNCTION: return nCustomFunctions_;
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
            case StatsCategory::APIFUNCTION: return apiFunctionsSet_.size();
            case StatsCategory::CUSTOMFUNCTION: return customFunctionsSet_.size();
            default: throw std::invalid_argument("Unknown CSSet type");
        }
    }

    CodeStatistics::StatSize CodeStatistics::getUniqueOperands() const  {
        return getCSSetSize(StatsCategory::CONSTANT) + getCSSetSize(StatsCategory::IDENTIFIER);
    }
    CodeStatistics::StatSize CodeStatistics::getUniqueOperators() const {
        return getCSSetSize(StatsCategory::CSPECIFIER) + getCSSetSize(StatsCategory::KEYWORD) + getCSSetSize(StatsCategory::OPERATOR) + getCSSetSize(StatsCategory::TYPE);
    }

    CodeStatistics::StatSize CodeStatistics::getOperands() const {
        return getCounterValue(StatsCategory::CONSTANT) + getCounterValue(StatsCategory::IDENTIFIER);
    }

    CodeStatistics::StatSize CodeStatistics::getOperators() const {
        return getCounterValue(StatsCategory::CSPECIFIER) + getCounterValue(StatsCategory::KEYWORD) + getCounterValue(StatsCategory::OPERATOR) + getCounterValue(StatsCategory::TYPE);
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
        nAPIFunctions_ = 0;
        nCustomFunctions_ = 0;
        // Reset all sets
        typesSet_.clear();
        constantsSet_.clear();
        identifiersSet_.clear();
        cSpecifiersSet_.clear();
        keywordsSet_.clear();
        operatorsSet_.clear();
        conditionsSet_.clear();
        apiFunctionsSet_.clear();
        customFunctionsSet_.clear();
    }

    void CodeStatistics::printMetrics(std::ostringstream& result, const CSSet& set, const int nameWidth, const int valueWidth) const {
        for (const auto& element : set) {
            result << std::left << std::setw(nameWidth) << element.first << " " 
                << std::right << std::setw(valueWidth) << element.second << '\n';
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
    std::string CodeStatistics::printCustomFunctions() const {
        const int nameWidth = 45; // Column width for metric names
        const int valueWidth = 15; // Adjusted column width for metric values
        const int totalWidth = 80; // Total width for both columns
        std::ostringstream result;
        // Adding header
        printHeader(result, "Custom Function Calls", "Occurrences", nameWidth, valueWidth, totalWidth);
        // Combining all operand sets into a single table
        printMetrics(result, customFunctionsSet_, nameWidth, valueWidth);

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
    std::string CodeStatistics::printAPIFunctions() const {
        const int nameWidth = 45; // Column width for metric names
        const int valueWidth = 15; // Adjusted column width for metric values
        const int totalWidth = 80; // Total width for both columns
        std::ostringstream result;
        // Adding header
        printHeader(result, "API Function Calls", "Occurrences", nameWidth, valueWidth, totalWidth);
        // Combining all operand sets into a single table
        printMetrics(result, apiFunctionsSet_, nameWidth, valueWidth);

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
        nAPIFunctions_ += rhs.nAPIFunctions_;
        nCustomFunctions_ += rhs.nCustomFunctions_;

        typesSet_.insert(rhs.typesSet_.begin(), rhs.typesSet_.end());
        constantsSet_.insert(rhs.constantsSet_.begin(), rhs.constantsSet_.end());
        identifiersSet_.insert(rhs.identifiersSet_.begin(), rhs.identifiersSet_.end());
        cSpecifiersSet_.insert(rhs.cSpecifiersSet_.begin(), rhs.cSpecifiersSet_.end());
        keywordsSet_.insert(rhs.keywordsSet_.begin(), rhs.keywordsSet_.end());
        operatorsSet_.insert(rhs.operatorsSet_.begin(), rhs.operatorsSet_.end());
        conditionsSet_.insert(rhs.conditionsSet_.begin(), rhs.conditionsSet_.end());
        apiFunctionsSet_.insert(rhs.apiFunctionsSet_.begin(), rhs.apiFunctionsSet_.end());
        customFunctionsSet_.insert(rhs.customFunctionsSet_.begin(), rhs.customFunctionsSet_.end());

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
            case StatsCategory::APIFUNCTION: return nAPIFunctions_;
            case StatsCategory::CUSTOMFUNCTION: return nCustomFunctions_;
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
            case StatsCategory::APIFUNCTION: return apiFunctionsSet_;
            case StatsCategory::CUSTOMFUNCTION: return customFunctionsSet_;
            default: throw std::invalid_argument("Unknown CSSet type");
        }
    }
}
