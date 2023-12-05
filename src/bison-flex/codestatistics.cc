#include "codestatistics.hh"
#include "parser.hh"
#include "scanner.hh"

namespace c3ms
{
    CodeStatistics::CodeStatistics()
        : scanner_(new CodeScanner()),
          parser_(new CodeParser(*this)),
          error_(0)
    {
    }

    CodeStatistics::~CodeStatistics()
    {
        delete parser_;
        delete scanner_;
    }

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

    int CodeStatistics::parse_file(std::string &path)
    {
        std::ifstream s(path.c_str(), std::ifstream::in);
        scanner_->switch_streams(&s, &std::cerr);
        parser_->parse();
        s.close();
        return error_;
    }

    void CodeStatistics::incrementCounter(StatsCategory counter, const std::string& p) {
        getCounterReference(counter)++;
        auto& setRef = getCSSetReference(counter);
        auto [it, inserted] = setRef.insert({p, 1});
        if (!inserted) {
            it->second++;
        }
    }

    CodeStatistics::StatSize CodeStatistics::getCounterValue(StatsCategory counter) {
        return getCounterReference(counter);
    }

    CodeStatistics::StatSize CodeStatistics::getCSSetSize(StatsCategory set) {
        return getCSSetReference(set).size();
    }

    CodeStatistics::StatSize& CodeStatistics::getCounterReference(StatsCategory counter) {
        switch (counter) {
            case TYPE: return nTypes_;
            case CONSTANT: return nConstants_;
            case IDENTIFIER: return nIdentifiers_;
            case CSPECIFIER: return nCSpecifiers_;
            case KEYWORD: return nKeywords_;
            case OPERATOR: return nOperators_;
            case CONDITION: return nConditions_;
            case APIFUNCTION: return nAPIFunctions_;
            case CUSTOMFUNCTION: return nCustomFunctions_;
            default: throw std::invalid_argument("Unknown counter type");
        }
    }

    CodeStatistics::CSSet& CodeStatistics::getCSSetReference(StatsCategory set) {
        switch (set) {
            case TYPE: return typesSet_;
            case CONSTANT: return constantsSet_;
            case IDENTIFIER: return identifiersSet_;
            case CSPECIFIER: return cSpecifiersSet_;
            case KEYWORD: return keywordsSet_;
            case OPERATOR: return operatorsSet_;
            case CONDITION: return conditionsSet_;
            case APIFUNCTION: return apiFunctionsSet_;
            case CUSTOMFUNCTION: return customFunctionsSet_;
            default: throw std::invalid_argument("Unknown CSSet type");
        }
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


    void CodeStatistics::printMetrics(std::ostringstream& result, const CSSet& set, const int nameWidth, const int valueWidth) const {
        for (const auto& element : set) {
            result << std::left << std::setw(nameWidth) << element.first << " " 
                << std::right << std::setw(valueWidth) << element.second << '\n';
        }
    }

    void CodeStatistics::printHeader(std::ostringstream& result, const std::string& left_header, const std::string& right_header, const int& nameWidth, const int& valueWidth, const int& totalWidth) const {
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

}
