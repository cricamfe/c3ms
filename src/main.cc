#include <iostream>
#include <fstream>
#include "bison-flex/codestatistics.hh"
#include "CodeMetrics.hpp"
#include "CodeUtils.hpp"

using namespace c3ms;

// Debugging flag to print additional information
#ifndef DEBUG
#   define DEBUG 0
#endif

int main(int argc, char* argv[]) {
    // Initialize variables for command-line arguments
    int verbosity = 1;  // Verbosity level for output
    bool fileMetricsFlag = false;
    bool functionMetricsFlag = false;
    bool globalMetricsFlag = false;
    bool printCodeFlag = false;

    // Parse command-line arguments and get file paths to analyze
    auto filepaths = parseArguments(argc, argv, verbosity, functionMetricsFlag, fileMetricsFlag, globalMetricsFlag, printCodeFlag);

    // Display usage information if no file paths are provided
    if (filepaths.empty()) {
        usage();
        return EXIT_FAILURE;
    }

    // Initialize global statistics
    CodeStatistics globalStats;
    CodeStatistics fileStats;
    CodeStatistics functionStats;
    int globalLinesOfCode = 0;

    // Process each file path provided
    for (const auto& filePath : filepaths) {
        // Check if the file is accessible and a regular file
        if (!std::filesystem::exists(filePath) || !std::filesystem::is_regular_file(filePath)) {
            std::cerr << "Error: " << filePath << " not accessible or invalid\n";
            continue; // Skip to the next file if this one is invalid
        }
        
        // Analyze functions in the file if the flag is set
        if (functionMetricsFlag) {
            int fileLinesOfCode = 0; // Initialize file line count
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
                    functionStats.parse_file(tempFilename);
                    int linesOfCodeFunc = countLinesOfCode(tempFilename.c_str());

                    // Calculate and report metrics for the function
                    MetricsCalculator metricsFunc(functionStats, linesOfCodeFunc);
                    if (functionMetricsFlag || (!fileMetricsFlag && !globalMetricsFlag)) {
                        printHeader("Function Metrics: " + func.name, RED);
                        metricsFunc.report(verbosity, func.name, linesOfCodeFunc, functionStats);
                    }

                    // Print the function contents if debugging is enabled
                    if constexpr (DEBUG) {
                        std::clog << "Function: " << func.name << " (" << linesOfCodeFunc << " lines)" << std::endl;
                        std::clog << "Lines of code: " << linesOfCodeFunc << std::endl;
                        if (printCodeFlag) { std::clog << func.code << std::endl; }
                        std::clog << functionStats.printOperators() << std::endl;
                        std::clog << functionStats.printOperands() << std::endl;
                        std::clog << functionStats.printAPIFunctions() << std::endl;
                        std::clog << functionStats.printCustomFunctions() << std::endl;
                    }

                    // Clean up the temporary file
                    deleteTemporaryFile(tempFilename);

                    // Aggregate function metrics into file metrics
                    fileStats += functionStats;
                    fileLinesOfCode += linesOfCodeFunc;

                    // Reset function statistics
                    functionStats.reset();
                } catch (const std::exception& e) {
                    // Handle and report any errors during function processing
                    std::cerr << "Error processing function " << func.name << ": " << e.what() << std::endl;
                }
            }

            // Calculate and report metrics for the entire file
            MetricsCalculator fileMetrics(fileStats, fileLinesOfCode);
            if (fileMetricsFlag || (!functionMetricsFlag && !globalMetricsFlag)) {
                printHeader("File Metrics: " + filePath.filename().string(), GREEN);
                fileMetrics.report(verbosity, filePath.filename().string(), fileLinesOfCode, fileStats);
            }

            // Aggregate file metrics into global metrics
            globalStats += fileStats;
            globalLinesOfCode += fileLinesOfCode;
        } else {
            // Process the entire file if function analysis is not required
            fileStats.parse_file(filePath.string());
            int fileLinesOfCode = countLinesOfCode(filePath.string());

            // Calculate and report metrics for the file
            MetricsCalculator metrics(fileStats, fileLinesOfCode);
            std::cout << "File: " << filePath.filename().string() << " (" << fileLinesOfCode << " lines)" << std::endl;
            if (fileMetricsFlag || (!functionMetricsFlag && !globalMetricsFlag)) {
                printHeader("File Metrics: " + filePath.filename().string(), GREEN);
                metrics.report(verbosity, filePath.filename().string(), fileLinesOfCode, fileStats);
            }

            // Print the file contents if debugging is enabled
            if constexpr (DEBUG) {
                std::clog << "File: " << filePath.filename().string() << " (" << fileLinesOfCode << " lines)" << std::endl;
                std::clog << "Lines of code: " << fileLinesOfCode << std::endl;
                if (printCodeFlag) {
                    std::ifstream file(filePath);
                    std::string line;
                    while (std::getline(file, line)) {
                        std::clog << line << std::endl;
                    }
                }
                std::clog << fileStats.printOperators() << std::endl;
                std::clog << fileStats.printOperands() << std::endl;
            }

            // Aggregate file metrics into global metrics
            globalStats += fileStats;
            globalLinesOfCode += fileLinesOfCode;

            // Reset file statistics
            fileStats.reset();
        }
    }

    // Calculate and report global metrics across all files
    MetricsCalculator globalMetrics(globalStats, globalLinesOfCode);
    if (globalMetricsFlag || (!fileMetricsFlag && !functionMetricsFlag)) {
        printHeader("Global Metrics", YELLOW);
        globalMetrics.report(verbosity, "Global", globalLinesOfCode, globalStats);
    }

    // Exit successfully
    return EXIT_SUCCESS;
}
