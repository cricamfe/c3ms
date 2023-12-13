#ifndef __CODESTATISTICS_HH_
#define __CODESTATISTICS_HH_

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string_view>
#include <iomanip>
#include <memory>


namespace c3ms
{
    /// Forward declarations of classes
    class CodeParser;
    class CodeScanner;
    class location;

    class CodeStatistics
    {
        public:
            enum class StatsCategory;
            using StatSize = std::size_t;
            using CSSet = std::unordered_map<std::string, std::pair<StatSize, StatsCategory>>;

            /**
             * @brief Enum class representing different categories for code statistics.
             */
            enum class StatsCategory {
                TYPE,           // Standard C++ types
                CONSTANT,       // Standard C++ constants
                IDENTIFIER,     // Standard C++ identifiers     
                CSPECIFIER,     // Standard C++ specifiers
                KEYWORD,        // Standard C++ keywords
                OPERATOR,       // Standard C++ operators
                CONDITION,      // Standard C++ if, else, else if, switch, case, default
                APIKEYWORD,     // API High Level
                APILLKEYWORD,   // API Low Level
                CUSTOMKEYWORD,  // Custom Keywords
            };

            // Constructors and Destructor
            CodeStatistics();

            // Public Member Functions
            int parse();
            int parse(std::istream& iss);
            int parse_file(const std::string& path);

            void category(StatsCategory counter, std::string_view p);
            StatSize getCounterValue(StatsCategory set) const;
            StatSize getCSSetSize(StatsCategory set) const;

            StatSize getUniqueOperands() const;
            StatSize getUniqueOperators() const;
            StatSize getOperands() const;
            StatSize getOperators() const;

            void decOperator();
            void addCondition();
            /**
             * @brief Resets the code statistics.
             * 
             * This function resets the code statistics to their initial values.
             */
            void reset();

            void printMetrics(std::ostringstream& result, const CSSet& set, int nameWidth, int valueWidth) const;
            void printHeader(std::ostringstream& result, std::string_view left_header, std::string_view right_header, int nameWidth, int valueWidth, int totalWidth) const;
            std::string printOperators() const;
            std::string printOperands() const;
            std::string printCustom() const;
            std::string printAPI() const;
            std::string printAPILowLevel() const;

            // Overloaded Operators
            CodeStatistics& operator+=(const CodeStatistics& rhs);

            // Get and Set Methods for error_
            int getError() const { return error_; }
            void setError(int value) { error_ = value; }

        private:
            // Private Member Functions
            StatSize& getCounterReference(StatsCategory counter);
            CSSet& getCSSetReference(StatsCategory set);
            std::string toString(StatsCategory category) const;

            // Member Variables
            std::shared_ptr<CodeScanner> scanner_;
            std::shared_ptr<CodeParser> parser_;
            int error_;

            StatSize nTypes_ = 0;
            StatSize nConstants_ = 0;
            StatSize nIdentifiers_ = 0;
            StatSize nCSpecifiers_ = 0;
            StatSize nKeywords_ = 0;
            StatSize nOperators_ = 0;
            StatSize nConditions_ = 0;
            StatSize nAPIKeywords_ = 0;
            StatSize nAPILLKeywords_ = 0;
            StatSize nCustomKeywords_ = 0;

            CSSet typesSet_;
            CSSet constantsSet_;
            CSSet identifiersSet_;
            CSSet cSpecifiersSet_;
            CSSet keywordsSet_;
            CSSet operatorsSet_;
            CSSet conditionsSet_;
            CSSet apiKeywordsSet_;
            CSSet apiLLKeywordsSet_;
            CSSet customKeywordsSet_;

            // Friends of CodeStatistics
            friend class CodeParser;
            friend class CodeScanner;
    };
}

#endif /* !CODESTATISTICS_HH_ */