/* Copyright 2023 Campos-Ferrer, Cristian. Universidad de Málaga */

# include "CodeMetrics.hpp"

// Constructor for MetricsCalculator class
MetricsCalculator::MetricsCalculator(const CodeStatistics& cs, const int lc) 
    : n1(cs.getUniqueOperators()), // Initialize unique operators count
      n2(cs.getUniqueOperands()), // Initialize unique operands count
      N1(cs.getOperators()), // Initialize total operators count
      N2(cs.getOperands()), // Initialize total operands count
      n(n1 + n2), // Calculate total unique entities (operators + operands)
      N(N1 + N2), // Calculate total entities (operators + operands)
      conditions(cs.getCounterValue(StatsCategory::CONDITION)), // Initialize conditional count
      linesOfCode(lc) // Set lines of code
{
    calculateMetrics(); // Calculate all metrics upon initialization
}

// Method to calculate various code metrics
void MetricsCalculator::calculateMetrics() {
    volume = N * log2(n); // Calculate program volume
    difficulty = (n1 / 2.0) * (N2 / static_cast<double>(n2)); // Calculate program difficulty
    effort = volume * difficulty; // Calculate programming effort
    timeRequired = effort / 18.0; // Calculate time required to develop
    numberOfBugs = pow(effort, 2.0 / 3.0) / 3000.0; // Estimate number of bugs
    cyclomaticComplexity = conditions + 1; // Calculate cyclomatic complexity
    maintainabilityIndex = 171 - 5.2 * log(volume) - 0.23 * cyclomaticComplexity - 16.2 * log(linesOfCode); // Calculate maintainability index
}

// Method to report the calculated metrics
void MetricsCalculator::report(int verbosity, const std::string& filePath, int loc, CodeStatistics& cs) const {
    std::ostringstream reportStream; // Stream to build the report
    const int nameWidth = 45; // Column width for metric names
    const int valueWidth = 15; // Column width for metric values

    // Lambda to format and return a metric as a string
    auto formatMetric = [&](const std::string& name, auto value, const std::string& extra = "") {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2); // Set fixed-point notation with 2 decimal places
        ss << std::left << std::setw(nameWidth) << name << " " 
           << std::right << std::setw(valueWidth) << value;
        if (!extra.empty()) {
            ss << " (" << extra << ")";
        }
        ss << '\n';
        return ss.str();
    };

    // Generate report based on verbosity level
    // Default verbosity level is 1
    reportStream << formatMetric("Effort", effort) << formatMetric("Volume", volume)
                    << formatMetric("Conditions", conditions) << formatMetric("Cyclomatic Complexity", cyclomaticComplexity)
                    << formatMetric("Difficulty", difficulty) << formatMetric("Time Required", timeRequired, "seconds")
                    << formatMetric("Bugs", numberOfBugs, "delivered") << formatMetric("Maintainability", maintainabilityIndex)
                    << std::string(80, '-') << "\n";

    if (verbosity > 2) {
        // Detailed metrics
        reportStream << "Detailed Metrics:\n" << std::string(80, '-') << "\n"
                     << formatMetric("n1 (unique operators)", n1) << formatMetric("n2 (unique operands)", n2)
                     << formatMetric("N1 (total # operators)", N1) << formatMetric("N2 (total # operands)", N2)
                     << std::string(80, '-') << "\n";
    }

    if (verbosity > 1) {
        // Additional statistics
        reportStream << "Additional Statistics:\n" << std::string(80, '-') << "\n";
        reportStream << formatMetric("Types", cs.getCounterValue(StatsCategory::TYPE), std::to_string(cs.getCSSetSize(StatsCategory::TYPE)) + " unique")
                        << formatMetric("Types (API)", cs.getCounterValue(StatsCategory::APITYPE), std::to_string(cs.getCSSetSize(StatsCategory::APITYPE)) + " unique")
                        << formatMetric("Types (API Low Level)", cs.getCounterValue(StatsCategory::APILLTYPE), std::to_string(cs.getCSSetSize(StatsCategory::APILLTYPE)) + " unique")
                        << formatMetric("Types (Dev)", cs.getCounterValue(StatsCategory::CUSTOMTYPE), std::to_string(cs.getCSSetSize(StatsCategory::CUSTOMTYPE)) + " unique")
                        << formatMetric("Constants", cs.getCounterValue(StatsCategory::CONSTANT), std::to_string(cs.getCSSetSize(StatsCategory::CONSTANT)) + " unique")
                        << formatMetric("Constants (API)", cs.getCounterValue(StatsCategory::APICONSTANT), std::to_string(cs.getCSSetSize(StatsCategory::APICONSTANT)) + " unique")
                        << formatMetric("Constants (API Low Level)", cs.getCounterValue(StatsCategory::APILLCONSTANT), std::to_string(cs.getCSSetSize(StatsCategory::APILLCONSTANT)) + " unique")
                        << formatMetric("Constants (Dev)", cs.getCounterValue(StatsCategory::CUSTOMCONSTANT), std::to_string(cs.getCSSetSize(StatsCategory::CUSTOMCONSTANT)) + " unique")
                        << formatMetric("Identifiers", cs.getCounterValue(StatsCategory::IDENTIFIER), std::to_string(cs.getCSSetSize(StatsCategory::IDENTIFIER)) + " unique")
                        << formatMetric("Cspecs", cs.getCounterValue(StatsCategory::CSPECIFIER), std::to_string(cs.getCSSetSize(StatsCategory::CSPECIFIER)) + " unique")
                        << formatMetric("Keywords", cs.getCounterValue(StatsCategory::KEYWORD), std::to_string(cs.getCSSetSize(StatsCategory::KEYWORD)) + " unique")
                        << formatMetric("Keywords (API)", cs.getCounterValue(StatsCategory::APIKEYWORD), std::to_string(cs.getCSSetSize(StatsCategory::APIKEYWORD)) + " unique")
                        << formatMetric("Keywords (API Low Level)", cs.getCounterValue(StatsCategory::APILLKEYWORD), std::to_string(cs.getCSSetSize(StatsCategory::APILLKEYWORD)) + " unique")
                        << formatMetric("Keywords (Dev)", cs.getCounterValue(StatsCategory::CUSTOMKEYWORD), std::to_string(cs.getCSSetSize(StatsCategory::CUSTOMKEYWORD)) + " unique")
                        << formatMetric("Operators", cs.getOperators(), std::to_string(cs.getUniqueOperators()) + " unique")
                        << formatMetric("Operands", cs.getOperands(), std::to_string(cs.getUniqueOperands()) + " unique")
                        << std::string(80, '-') << "\n";
    }

    // Output the final report
    std::cout << reportStream.str();
}


// Method to return the calculated volume
double MetricsCalculator::getVolume() const { return volume; } // Returns the volume of the program

// Method to return the calculated cyclomatic complexity
int MetricsCalculator::getCyclomaticComplexity() const { return cyclomaticComplexity; } // Returns the cyclomatic complexity

// Method to return the total number of tokens
int MetricsCalculator::getLinesOfCode() const { return linesOfCode; } // Returns the total number of lines of code