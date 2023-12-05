#ifndef __CODESTATISTICS_HH_
#define __CODESTATISTICS_HH_

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string_view>
#include <iomanip>
#include <cstring>

namespace c3ms
{
    /// Forward declarations of classes
    class CodeParser;
    class CodeScanner;
    class location;

    class CodeStatistics
    {
        public:
            using StatSize = std::size_t;
            using CSSet = std::unordered_map<std::string, StatSize>;

            enum StatsCategory {
                TYPE,
                CONSTANT,
                IDENTIFIER,
                CSPECIFIER,
                KEYWORD,
                OPERATOR,
                CONDITION,
                APIFUNCTION,
                CUSTOMFUNCTION,
            };

            CodeStatistics();
            ~CodeStatistics();

            int parse();
            int parse(std::istream &iss);
            int parse_file(std::string& path);

            void incrementCounter(StatsCategory counter, const std::string& p);
            StatSize getCounterValue(StatsCategory counter);
            StatSize getCSSetSize(StatsCategory set);

            void reset();

            // Operator overloading
            CodeStatistics& operator+=(const CodeStatistics& rhs);
            
            // Print functions
            void printMetrics(std::ostringstream& result, const CSSet& set, const int nameWidth, const int valueWidth) const;
            void printHeader(std::ostringstream& result, const std::string& left_header, const std::string& right_header, const int& nameWidth, const int& valueWidth, const int& totalWidth) const;
            std::string printOperators() const;

            void printTypeStats() const {
                printf("Types: %lu\n", nTypes_);
                for (const auto& element : typesSet_) {
                    printf("%s %lu\n", element.first.c_str(), element.second);
                }
            }

            void type(std::string p)       { nTypes_++;       typesSet_[std::string(p)]++; }

        private:
            CodeScanner* scanner_;
            CodeParser* parser_;
            int error_;

            StatSize nTypes_ = 0;
            StatSize nConstants_ = 0;
            StatSize nIdentifiers_ = 0;
            StatSize nCSpecifiers_ = 0;
            StatSize nKeywords_ = 0;
            StatSize nOperators_ = 0;
            StatSize nConditions_ = 0;
            StatSize nAPIFunctions_ = 0;
            StatSize nCustomFunctions_ = 0;

            CSSet typesSet_;
            CSSet constantsSet_;
            CSSet identifiersSet_;
            CSSet cSpecifiersSet_;
            CSSet keywordsSet_;
            CSSet operatorsSet_;
            CSSet conditionsSet_;
            CSSet apiFunctionsSet_;
            CSSet customFunctionsSet_;

            StatSize& getCounterReference(StatsCategory counter);
            CSSet& getCSSetReference(StatsCategory set);

            /// Allows Parser and Scanner to access private attributes of the CodeStatistics class
            friend class CodeParser;
            friend class CodeScanner;
    };
}

#endif /* !CODESTATISTICS_HH_ */

