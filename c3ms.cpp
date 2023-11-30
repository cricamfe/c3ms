/* Copyright (c) 2009, 2010 Basilio B. Fraguela. Universidade da Coruña */
/* Upgrade (2023) by Campos-Ferrer, Cristian. Universidad de Málaga */

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <fstream>
#include <regex>
#include <cstring>
#include <unistd.h>
#include <stdexcept>

#include "CodeStatistics.h"
#include "CodeMetrics.hpp"
#include "CodeUtils.hpp"

// Debugging flag to print additional information
#ifndef DEBUG
#define DEBUG 0
#endif

extern const CodeStatistics& readFile(const char* fname);

int main(int argc, char* argv[]) {
    // Initialize variables for command-line arguments
    int verbosity = 1;  // Verbosity level for output
    bool fileMetricsFlag = false;
    bool functionMetricsFlag = false;
    bool globalMetricsFlag = false;

    // Parse command-line arguments and get file paths to analyze
    auto filepaths = parseArguments(argc, argv, verbosity, functionMetricsFlag, fileMetricsFlag, globalMetricsFlag);

    // Display usage information if no file paths are provided
    if (filepaths.empty()) {
        usage();
        return EXIT_FAILURE;
    }

    // Initialize global statistics
    CodeStatistics globalCs;
    int globalLinesOfCode = 0;

    // Process each file path provided
    for (const auto& filePath : filepaths) {
        // Check if the file is accessible and a regular file
        if (!std::filesystem::exists(filePath) || !std::filesystem::is_regular_file(filePath)) {
            std::cerr << "Error: " << filePath << " not accessible or invalid\n";
            continue; // Skip to the next file if this one is invalid
        }
        
        // Initialize file statistics
        CodeStatistics fileCS; // Initialize code statistics for the file
        int fileLinesOfCode = 0; // Initialize line count for the file

        // Analyze functions in the file if the flag is set
        if (functionMetricsFlag) {
            auto functions = extractFunctions(filePath.string()); // Extract functions from the file
            if constexpr (DEBUG) {
                std::clog << "Functions: " << std::endl;
                for (const auto& func : functions) {
                    std::clog << func.name << std::endl;
                }
            }
            // Process each function in the file
            for (const auto& func : functions) {
                try {
                    // Create a temporary file with the function's code
                    std::string tempFilename = createTemporaryFile(func.code, func.name);
                    // Read statistics and line count for the function
                    const CodeStatistics& csFunc = readFile(tempFilename.c_str());
                    int linesOfCodeFunc = countLinesOfCode(tempFilename.c_str());

                    // Calculate and report metrics for the function
                    MetricsCalculator metricsFunc(csFunc, linesOfCodeFunc);
                    if (functionMetricsFlag || (!fileMetricsFlag && !globalMetricsFlag)) {
                        printHeader("Function Metrics: " + func.name, RED);
                        metricsFunc.report(verbosity, func.name, linesOfCodeFunc, csFunc);
                    }

                    // Print the function contents if debugging is enabled
                    if constexpr (DEBUG) {
                        std::clog << "Function: " << func.name << " (" << linesOfCodeFunc << " lines)" << std::endl;
                        std::clog << func.code << std::endl;
                        std::clog << "Lines of code: " << linesOfCodeFunc << std::endl;
                    }

                    // Clean up the temporary file
                    deleteTemporaryFile(tempFilename);

                    // Aggregate function metrics into file metrics
                    fileCS += csFunc;
                    fileLinesOfCode += linesOfCodeFunc;
                } catch (const std::exception& e) {
                    // Handle and report any errors during function processing
                    std::cerr << "Error processing function " << func.name << ": " << e.what() << std::endl;
                }
            }

            // Calculate and report metrics for the entire file
            MetricsCalculator fileMetrics(fileCS, fileLinesOfCode);
            if (fileMetricsFlag || (!functionMetricsFlag && !globalMetricsFlag)) {
                printHeader("File Metrics: " + filePath.filename().string(), GREEN);
                fileMetrics.report(verbosity, filePath.filename().string(), fileLinesOfCode, fileCS);
            }

            // Aggregate file metrics into global metrics
            globalCs += fileCS;
            globalLinesOfCode += fileLinesOfCode;
        } else {
            // Process the entire file if function analysis is not required
            const CodeStatistics& cs = readFile(filePath.string().c_str());
            int linesOfCode = countLinesOfCode(filePath.string());

            // Calculate and report metrics for the file
            MetricsCalculator metrics(cs, linesOfCode);
            if (fileMetricsFlag || (!functionMetricsFlag && !globalMetricsFlag)) {
                printHeader("File Metrics: " + filePath.filename().string(), GREEN);
                metrics.report(verbosity, filePath.filename().string(), linesOfCode, cs);
            }

            // Print the file contents if debugging is enabled
            if constexpr (DEBUG) {
                std::clog << "File: " << filePath.filename().string() << " (" << linesOfCode << " lines)" << std::endl;
                std::ifstream file(filePath);
                std::string line;
                while (std::getline(file, line)) {
                    std::clog << line << std::endl;
                }
                std::clog << "Lines of code: " << linesOfCode << std::endl;
            }

            // Aggregate file metrics into global metrics
            globalCs += cs;
            globalLinesOfCode += linesOfCode;
        }
    }

    // Calculate and report global metrics across all files
    MetricsCalculator globalMetrics(globalCs, globalLinesOfCode);
    if (globalMetricsFlag || (!fileMetricsFlag && !functionMetricsFlag)) {
        printHeader("Global Metrics", YELLOW);
        globalMetrics.report(verbosity, "Global", globalLinesOfCode, globalCs);
    }

    // Exit successfully
    return EXIT_SUCCESS;
}