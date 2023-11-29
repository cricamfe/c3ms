/* Copyright 2023 Campos-Ferrer, Cristian. Universidad de Málaga */

#ifndef CODE_METRICS_HPP
#define CODE_METRICS_HPP

#include <string>
#include <vector>
#include <filesystem>
#include <cmath>
#include <iostream>

#include "CodeStatistics.h"

/**
 * @class MetricsCalculator
 * 
 * @brief A class that calculates various software metrics.
 * 
 * @details This class calculates various software metrics based on the statistics of a piece of code. 
 * The metrics calculated include Halstead metrics (volume, difficulty, effort, time required, number of bugs), 
 * maintainability index, cyclomatic complexity, and lines of code. 
 * The class provides a method to report these metrics in a formatted way.
 * 
 * @member n1 Number of unique operators.
 * @member n2 Number of unique operands.
 * @member N1 Total number of operators.
 * @member N2 Total number of operands.
 * @member n Total number of unique tokens (n1 + n2).
 * @member N Total number of tokens (N1 + N2).
 * @member volume Halstead volume.
 * @member difficulty Halstead difficulty.
 * @member effort Halstead effort.
 * @member timeRequired Time required to program.
 * @member numberOfBugs Estimated number of bugs.
 * @member conditions Number of conditions in the code.
 * @member maintainabilityIndex Maintainability index.
 * @member cyclomaticComplexity Cyclomatic complexity.
 * @member linesOfCode Lines of code.
 * 
 * @method MetricsCalculator Constructor that initializes the metrics based on the given code statistics and lines of code.
 * @method calculateMetrics Calculates the metrics.
 * @method report Generates a report of the metrics.
 * @method getVolume Returns the Halstead volume.
 * @method getCyclomaticComplexity Returns the cyclomatic complexity.
 * @method getLinesOfCode Returns the total number of tokens.
 */
class MetricsCalculator {
public:
    MetricsCalculator(const CodeStatistics& cs, int lc);
    void calculateMetrics();
    void report(int verbosity, const std::string& filePath, int loc, const CodeStatistics& cs) const;
    double getVolume() const;
    int getCyclomaticComplexity() const;
    int getLinesOfCode() const;

private:
    unsigned int n1, n2, N1, N2, n, N;
    double volume, difficulty, effort, timeRequired, numberOfBugs;
    int conditions, cyclomaticComplexity, linesOfCode, maintainabilityIndex;
};

#endif