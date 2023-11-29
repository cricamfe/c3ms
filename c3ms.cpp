/* Copyright (c) 2009, 2010 Basilio B. Fraguela. Universidade da Coruña */

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

const std::string RED = "\033[31m";     // Funciones
const std::string GREEN = "\033[32m";   // Archivos
const std::string YELLOW = "\033[33m";  // Global
const std::string RESET = "\033[0m";    // Restablecer al color por defecto

struct FunctionCode {
    std::string name;
    std::string code;
};

std::string createTemporaryFile(const std::string& functionCode, const std::string& baseName) {
    std::string tempFilename = "/tmp/" + baseName + "XXXXXX";
    std::vector<char> tempFilenameVector(tempFilename.begin(), tempFilename.end());
    tempFilenameVector.push_back('\0');

    int fd = mkstemp(tempFilenameVector.data());
    if (fd == -1) {
        std::cerr << "Error creating temporary file. Template: " << tempFilename << std::endl; // Agregado para depuración
        throw std::runtime_error("Unable to create temporary file");
    }

    FILE* tempFile = fdopen(fd, "w");
    if (tempFile == nullptr) {
        close(fd);
        std::cerr << "Failed to open file stream for: " << tempFilename << std::endl; // Agregado para depuración
        throw std::runtime_error("Failed to open file stream");
    }

    fputs(functionCode.c_str(), tempFile);
    fclose(tempFile);

    return std::string(tempFilenameVector.begin(), tempFilenameVector.end() - 1);
}

void deleteTemporaryFile(const std::string& filename) {
    std::remove(filename.c_str());
}

void printHeader(const std::string& title, const std::string& color) {
    std::cout << color; // Establecer el color
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << title << "\n";
    std::cout << std::string(60, '=') << "\n";
    std::cout << RESET; // Restablecer el color
}

class MetricsCalculator {
private:
	// Halstead Metrics
    unsigned int n1, n2, N1, N2, n, N;
    double volume, difficulty, effort, timeRequired, numberOfBugs;
    int conditions;
	// Maintainability Index
    double maintainabilityIndex;
	// Cyclomatic Complexity
    int cyclomaticComplexity;
	// Lines of Code
	int linesOfCode;

public:
    MetricsCalculator(const CodeStatistics& cs, const int lc) {
        n1 = cs.getUniqueOperators();
        n2 = cs.getUniqueOperands();
        N1 = cs.getOperators();
        N2 = cs.getOperands();
        n = n1 + n2;
        N = N1 + N2;
        conditions = cs.getConds();
		linesOfCode = lc;
        calculateMetrics();
    }

    void calculateMetrics() {
        volume = N * log2(n);
        difficulty = (n1 / 2.0) * (N2 / static_cast<double>(n2));
        effort = volume * difficulty;
        timeRequired = effort / 18.0;
        numberOfBugs = pow(effort, 2.0 / 3.0) / 3000.0;
        cyclomaticComplexity = conditions + 1;
        maintainabilityIndex = 171 - 5.2 * log(volume) - 0.23 * cyclomaticComplexity - 16.2 * log(linesOfCode);

    }

    void report(int verbosity, const std::string& filePath, const int linesOfCode, const CodeStatistics& cs) const {
        std::ostringstream reportStream;
        const int nameWidth = 30;
        const int valueWidth = 15;

        auto formatMetric = [&](const std::string& name, auto value, const std::string& extra = "") {
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(2); // Añade estas líneas
            ss << std::left << std::setw(nameWidth) << name << " " 
            << std::right << std::setw(valueWidth) << value;
            if (!extra.empty()) {
                ss << " (" << extra << ")";
            }
            ss << '\n';
            return ss.str();
        };

        if (verbosity > 0) {
            reportStream << formatMetric("Effort", effort);
            reportStream << formatMetric("Volume", volume);
            reportStream << formatMetric("Conditions", conditions);
            reportStream << formatMetric("Cyclomatic Complexity", cyclomaticComplexity);
            reportStream << formatMetric("Difficulty", difficulty);
            reportStream << formatMetric("Time Required", timeRequired, "seconds");
            reportStream << formatMetric("Bugs", numberOfBugs, "delivered");
            reportStream << formatMetric("Maintainability", maintainabilityIndex);
            reportStream << std::string(60, '-') << "\n";
        }

        if (verbosity > 2) {
            reportStream << "Detailed Metrics:\n";
            reportStream << std::string(60, '-') << "\n";
            reportStream << formatMetric("n1 (unique operators)", n1)
                         << formatMetric("n2 (unique operands)", n2)
                         << formatMetric("N1 (total # operators)", N1)
                         << formatMetric("N2 (total # operands)", N2);
            reportStream << std::string(60, '-') << "\n";
        }

        if (verbosity > 1) {
            reportStream << "Additional Statistics:\n";
            reportStream << std::string(60, '-') << "\n";
            reportStream << formatMetric("Types", cs.getTypes(), std::to_string(cs.getUniqueTypes()) + " unique")
                            << formatMetric("Constants", cs.getConstants(), std::to_string(cs.getUniqueConstants()) + " unique")
                            << formatMetric("Identifiers", cs.getIdentifiers(), std::to_string(cs.getUniqueIdentifiers()) + " unique")
                            << formatMetric("Cspecs", cs.getCspecs(), std::to_string(cs.getUniqueCspecs()) + " unique")
                            << formatMetric("Keywords", cs.getKeywords(), std::to_string(cs.getUniqueKeywords()) + " unique")
                            << formatMetric("Operators", cs.getOps(), std::to_string(cs.getUniqueOps()) + " unique");
            reportStream << std::string(60, '-') << "\n";
        }

        std::cout << reportStream.str();
    }

    // Accessor methods for global calculation
    double getVolume() const { return volume; }
    int getCyclomaticComplexity() const { return cyclomaticComplexity; }
    int getLinesOfCode() const { return N; }
};

// Separador de líneas
const std::string SEPARATOR = std::string(20, '-');

extern const CodeStatistics& readFile(const char* fname);

void usage() {
    std::cout << "C++ Code Complexity Measurement System\n"
              << "Usage: c3ms [-h] [-f] [-v level] <files>\n"
              << "-h, --help    Show this help message\n"
              << "-f, --methods Analyze functions instead of files\n"
              << "--v, --verbosity [level] Set verbosity level (0-3)\n";
    std::exit(EXIT_FAILURE);
}

std::vector<std::filesystem::path> parseArguments(int argc, char* argv[], int& verbosity, bool& analyzeFunctions) {
    std::vector<std::filesystem::path> filepaths;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-v" || arg == "--verbosity") && i + 1 < argc) {
            verbosity = std::stoi(argv[++i]);
        } else if (arg == "-f" || arg == "--function") {
            analyzeFunctions = true;
        } else if (arg == "-h" || arg == "--help") {
            usage();
        } else {
            filepaths.emplace_back(argv[i]);
        }
    }
    return filepaths;
}

std::vector<FunctionCode> extractFunctions(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string line;
    std::vector<FunctionCode> functions;
    std::ostringstream currentFunction;
    bool inFunction = false;
    int braceCount = 0;

    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo: " << filePath << std::endl;
        return functions;
    }

    std::regex functionPattern(
        R"(([\w\:\s\*\<\>\&\[\]]+\s+[\w\:\s\*\<\>\&\[\]]+\s*\([^)]*\)\s*(const)?\s*(noexcept)?\s*(override)?\s*)[\s\S]*?)"
    );
    std::regex namePattern(R"(\b(\w+)\s*\([^)]*\)\s*\{)");

    while (std::getline(file, line)) {
        if (inFunction) {
            braceCount += std::count(line.begin(), line.end(), '{');
            braceCount -= std::count(line.begin(), line.end(), '}');

            if (braceCount == 0 && line.find("}") != std::string::npos) {
                inFunction = false;
                currentFunction << line << "\n";
                std::string funcStr = currentFunction.str(); // Guardar el resultado en una variable
                std::smatch nameMatch;
                if (std::regex_search(funcStr, nameMatch, namePattern)) { // Usar la variable en lugar de un rvalue
                    functions.push_back({nameMatch[1], funcStr});
                }
                currentFunction.str("");
                currentFunction.clear();
            }
        } else {
            std::smatch matches;
            if (std::regex_search(line, matches, functionPattern)) {
                inFunction = true;
                braceCount = std::count(line.begin(), line.end(), '{');
                currentFunction.str("");
                currentFunction.clear();
            }
        }

        if (inFunction) {
            currentFunction << line << "\n";
        }
    }

    return functions;
}

int countLinesOfCode(const std::string& filePath) {
	std::ifstream file(filePath);
	if (!file) {
		std::cerr << "Error trying to open file: " << filePath << std::endl;
		return -1;
	}
	return std::count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n');
}

int main(int argc, char* argv[]) {
    int verbosity = 0;
    bool analyzeFunctions = false;
    auto filepaths = parseArguments(argc, argv, verbosity, analyzeFunctions);

    if (filepaths.empty()) {
        usage();
    }

    CodeStatistics globalCs;
    double globalVolume = 0;
    int globalCyclomaticComplexity = 0, globalLinesOfCode = 0;

    for (const auto& filePath : filepaths) {
        if (!std::filesystem::exists(filePath) || !std::filesystem::is_regular_file(filePath)) {
            std::cerr << "Error: " << filePath << " not accessible or invalid\n";
            continue;
        }

        if (analyzeFunctions) {
            // Internal variables
            CodeStatistics fileCS;
            double fileVolume = 0;
            int fileCyclomaticComplexity = 0, fileLinesOfCode = 0;
            int globalLinesOfCode = 0;

            auto functions = extractFunctions(filePath.string());
            for (const auto& func : functions) {
                try {
                    std::string tempFilename = createTemporaryFile(func.code, func.name);
                    const CodeStatistics& csFunc = readFile(tempFilename.c_str());
                    int linesOfCodeFunc = countLinesOfCode(tempFilename);
                    MetricsCalculator metricsFunc(csFunc, linesOfCodeFunc);
                    printHeader("Function Metrics: " + func.name, RED);
                    metricsFunc.report(verbosity, func.name, linesOfCodeFunc, csFunc);
                    deleteTemporaryFile(tempFilename);

                    fileCS += csFunc;
                    fileVolume += metricsFunc.getVolume();
                    fileCyclomaticComplexity += metricsFunc.getCyclomaticComplexity();
                    fileLinesOfCode += linesOfCodeFunc;
                } catch (const std::exception& e) {
                    std::cerr << "Error processing function " << func.name << ": " << e.what() << std::endl;
                }
            }
            MetricsCalculator fileMetrics(fileCS, fileLinesOfCode);
            if (verbosity > 0) {
                printHeader("File Metrics: " + filePath.filename().string(), GREEN);
                fileMetrics.report(verbosity, filePath.filename().string(), fileLinesOfCode, fileCS);
            }

            globalCs += fileCS;
            globalVolume += fileMetrics.getVolume();
            globalCyclomaticComplexity += fileMetrics.getCyclomaticComplexity();
            globalLinesOfCode += fileLinesOfCode;
        } else {
            const CodeStatistics& cs = readFile(filePath.string().c_str());
            int linesOfCode = countLinesOfCode(filePath.string());
            MetricsCalculator metrics(cs, linesOfCode);
            printHeader("File Metrics: " + filePath.filename().string(), GREEN);
            metrics.report(verbosity, filePath.filename().string(), linesOfCode, cs);

            globalCs += cs;
            globalVolume += metrics.getVolume();
            globalCyclomaticComplexity += metrics.getCyclomaticComplexity();
            globalLinesOfCode += linesOfCode;
        }
    }

    MetricsCalculator globalMetrics(globalCs, globalLinesOfCode);
    if (verbosity > 0) {
        printHeader("Global Metrics", YELLOW);
        globalMetrics.report(verbosity, "Global", globalLinesOfCode, globalCs);
    }

    return EXIT_SUCCESS;
}
