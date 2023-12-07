#include <iostream>
#include <fstream>
#include "bison-flex/codestatistics.hh"
#include "CodeMetrics.hpp"
#include "CodeUtils.hpp"

using namespace c3ms;

// Debugging flag to print additional information
#ifndef DEBUG
#define DEBUG 0
#endif

// Print debug information
void printDebugInfo(const std::string& header, const CodeStatistics& stats, int linesOfCode, bool printCodeFlag, const std::string& code = "") {
  if constexpr (DEBUG) {
    std::clog << header << " (" << linesOfCode << " lines)" << std::endl;
    std::clog << stats.printOperators() << std::endl;
    std::clog << stats.printOperands() << std::endl;
    std::clog << stats.printAPIFunctions() << std::endl;
    std::clog << stats.printCustomFunctions() << std::endl;
    if (printCodeFlag && !code.empty()) {
      std::clog << code << std::endl;
    }
  }
}


void processFile(const std::filesystem::path& filePath, int verbosity, bool fileMetricsFlag, bool globalMetricsFlag, bool printCodeFlag, CodeStatistics& globalStats, int& globalLinesOfCode) {
    CodeStatistics fileStats;
    fileStats.parse_file(filePath.string());
    int fileLinesOfCode = countLinesOfCode(filePath.string());

    // Calculate metrics
    MetricsCalculator fileMetrics(fileStats, fileLinesOfCode);
    if (fileMetricsFlag || (!globalMetricsFlag)) {
        printHeader("File Metrics: " + filePath.filename().string(), GREEN);
        fileMetrics.report(verbosity, filePath.filename().string(), fileLinesOfCode, fileStats);
    }

    // Update global stats
    globalStats += fileStats;
    globalLinesOfCode += fileLinesOfCode;

    printDebugInfo("File: " + filePath.filename().string(), fileStats, fileLinesOfCode, printCodeFlag);
}

void processFunction(const std::filesystem::path& filePath, int verbosity, bool functionMetricsFlag, bool fileMetricsFlag, bool globalMetricsFlag, bool printCodeFlag, CodeStatistics& globalStats, int& globalLinesOfCode) {
    CodeStatistics fileStats, functionStats;
    int fileLinesOfCode = 0;

    // Extract functions
    auto functions = extractFunctions(filePath.string());

    if constexpr (DEBUG) {
        std::clog << "Functions: " << std::endl;
        for (const auto& func : functions) {
            std::clog << func.name << std::endl;
        }
    }


    for (const auto& func : functions) {
        try {
            // Create and process temporary file
            std::string tempFilename = createTemporaryFile(func.code, func.name);
            functionStats.parse_file(tempFilename);
            int linesOfCodeFunc = countLinesOfCode(tempFilename.c_str());

            // Calculate function metrics
            MetricsCalculator metricsFunc(functionStats, linesOfCodeFunc);
            if (functionMetricsFlag || (!fileMetricsFlag && !globalMetricsFlag)) {
                printHeader("Function Metrics: " + func.name, RED);
                metricsFunc.report(verbosity, func.name, linesOfCodeFunc, functionStats);
            }

            // Update stats and print debug info
            printDebugInfo("Function: " + func.name, functionStats, linesOfCodeFunc, printCodeFlag, func.code);
            fileStats += functionStats;
            fileLinesOfCode += linesOfCodeFunc;
            functionStats.reset();

            // Delete temporary file
            deleteTemporaryFile(tempFilename);
        } catch (const std::exception& e) {
            std::cerr << "Error processing function " << func.name << ": " << e.what() << std::endl;
        }
    }

    // Calculate file metrics if needed
    MetricsCalculator metricsFile(fileStats, fileLinesOfCode);
    if (fileMetricsFlag || (!functionMetricsFlag && !globalMetricsFlag)) {
        printHeader("File Metrics: " + filePath.filename().string(), GREEN);
        metricsFile.report(verbosity, filePath.filename().string(), fileLinesOfCode, fileStats);
    }

    // Update global stats
    globalStats += fileStats;
    globalLinesOfCode += fileLinesOfCode;
}

int main(int argc, char* argv[]) {
    int verbosity = 1;
    bool fileMetricsFlag = false;
    bool functionMetricsFlag = false;
    bool globalMetricsFlag = false;
    bool printCodeFlag = false;

    auto filepaths = parseArguments(argc, argv, verbosity, functionMetricsFlag, fileMetricsFlag, globalMetricsFlag, printCodeFlag);

    if (filepaths.empty()) {
        usage();
        return EXIT_FAILURE;
    }

    CodeStatistics globalStats;
    int globalLinesOfCode = 0;

    for (const auto& filePath : filepaths) {
        if (!std::filesystem::exists(filePath) || !std::filesystem::is_regular_file(filePath)) {
            std::cerr << "Error: " << filePath << " not accessible or invalid\n";
            continue;
        }
        
        if (functionMetricsFlag) {
            processFunction(filePath, verbosity, functionMetricsFlag, fileMetricsFlag, globalMetricsFlag, printCodeFlag, globalStats, globalLinesOfCode);
        } else {
            processFile(filePath, verbosity, fileMetricsFlag, globalMetricsFlag, printCodeFlag, globalStats, globalLinesOfCode);
        }
    }

    MetricsCalculator globalMetrics(globalStats, globalLinesOfCode);

    if (globalMetricsFlag || (!fileMetricsFlag && !functionMetricsFlag)) {
        printHeader("Global Metrics", YELLOW);
        globalMetrics.report(verbosity, "Global", globalLinesOfCode, globalStats);
    }

    return EXIT_SUCCESS;
}