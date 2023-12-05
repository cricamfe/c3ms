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
 */
class MetricsCalculator {
public:
    /**
     * @brief Constructor that initializes the metrics based on the given code statistics and lines of code.
     * 
     * @param cs The code statistics to use for the calculations.
     * @param lc The number of lines of code.
     */
    MetricsCalculator(const CodeStatistics& cs, int lc);

    /**
     * @brief Calculates the metrics.
     */
    void calculateMetrics();

    /**
     * @brief Generates a report of the metrics.
     * 
     * @param verbosity The level of verbosity for the report.
     * @param filePath The path of the file for which the metrics are calculated.
     * @param loc The number of lines of code.
     * @param cs The code statistics.
     */
    void report(int verbosity, const std::string& filePath, int loc, const CodeStatistics& cs) const;

        /**
         * @brief Returns the Halstead volume.
         * 
         * @return The Halstead volume.
         */
        double getVolume() const;

        /**
         * @brief Returns the cyclomatic complexity.
         * 
         * @return The cyclomatic complexity.
         */
        int getCyclomaticComplexity() const;

        /**
         * @brief Returns the total number of tokens.
         * 
         * @return The total number of tokens.
         */
        int getLinesOfCode() const;

    private:
        unsigned int n1; ///< Number of unique operators.
        unsigned int n2; ///< Number of unique operands.
        unsigned int N1; ///< Total number of operators.
        unsigned int N2; ///< Total number of operands.
        unsigned int n; ///< Total number of unique tokens (n1 + n2).
        unsigned int N; ///< Total number of tokens (N1 + N2).
        double volume; ///< Halstead volume.
        double difficulty; ///< Halstead difficulty.
        double effort; ///< Halstead effort.
        double timeRequired; ///< Time required to program.
        double numberOfBugs; ///< Estimated number of bugs.
        int conditions; ///< Number of conditions in the code.
        int cyclomaticComplexity; ///< Cyclomatic complexity.
        int linesOfCode; ///< Lines of code.
        int maintainabilityIndex; ///< Maintainability index.
};

#endif // CODE_METRICS_HPP