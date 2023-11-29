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
#include "CodeStatistics.h"
// Simplified metric reporting
template<typename T>
void reportMetric(const char* name, const T& value) {
    std::cout << "  " << name << " : " << std::setw(9) << value << '\n';
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

    // template<typename T>
    // std::string formatMetric(const char* name, const T& value) const {
    //     std::ostringstream ss;
    //     ss << std::left << "  " << std::setw(30) << name << " : " << std::right << std::setw(10) << value << '\n';
    //     return ss.str();
    // }

    std::string reportStat(const char* name, int total, int unique) const {
        std::ostringstream ss;
        ss << std::left << "  " << std::setw(30) << name << " : " << std::right << std::setw(10) << total 
           << " (" << unique << " unique)\n";
        return ss.str();
    }

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

        if (verbosity > 0) {
            reportStream << filePath << ": (Effort: " << std::fixed << std::setprecision(2) << effort
                         << ") (Volume: " << volume << ") (Conditions: " << conditions
                         << ") (Cyclomatic Complexity: " << cyclomaticComplexity
                         << ") (Difficulty: " << difficulty << ") (Time Required: " << timeRequired
                         << ") (Bugs: " << numberOfBugs << ") (Maintainability: " << maintainabilityIndex << ")\n";
        }

        if (verbosity > 2) {
			auto formatMetric = [&](const char* name, const auto& value) {
				std::ostringstream ss;
				ss << std::left << "  " << std::setw(25) << name << " : " << std::right << std::setw(7) << value << '\n';
				return ss.str();
			};
            reportStream << formatMetric("n1 (unique operators)", n1)
                         << formatMetric("n2 (unique operands)", n2)
                         << formatMetric("N1 (total # operators)", N1)
                         << formatMetric("N2 (total # operands)", N2);
        }

        if (verbosity > 1) {
			auto reportStat = [&](const char* name, int total, int unique) {
				std::ostringstream ss;
				ss << std::left << "  " << std::setw(25) << name << " : " << std::right << std::setw(7) << total 
				   << " (" << unique << " unique)\n";
				return ss.str();
			};
            reportStream << reportStat("Types", cs.getTypes(), cs.getUniqueTypes())
                         << reportStat("Constants", cs.getConstants(), cs.getUniqueConstants())
                         << reportStat("Identifiers", cs.getIdentifiers(), cs.getUniqueIdentifiers())
                         << reportStat("Cspecs", cs.getCspecs(), cs.getUniqueCspecs())
                         << reportStat("Keywords", cs.getKeywords(), cs.getUniqueKeywords())
                         << reportStat("Operators", cs.getOps(), cs.getUniqueOps());
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

std::vector<std::filesystem::path> parseArguments(int argc, char* argv[], int& verbosity) {
    std::vector<std::filesystem::path> filepaths;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-v" && i + 1 < argc) {
            verbosity = std::stoi(argv[++i]);
        } else {
            filepaths.emplace_back(argv[i]);
        }
    }
    return filepaths;
}

int countLinesOfCode(const std::string& filePath) {
	std::ifstream file(filePath);
	if (!file) {
		std::cerr << "Error trying to open file: " << filePath << std::endl;
		return -1;
	}
	return std::count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n');
}

void usage() {
    std::cout << "C++ Code Complexity Measurement System\n"
              << "Usage: c3ms [-v level] <files>\n"
              << "-v level      Set verbosity level\n";
    std::exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    int verbosity = 0;
    auto filepaths = parseArguments(argc, argv, verbosity);

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

        const CodeStatistics& cs = readFile(filePath.string().c_str());
        int linesOfCode = countLinesOfCode(filePath.string());
		MetricsCalculator metrics(cs, linesOfCode);
        metrics.report(verbosity, filePath.filename().string(), linesOfCode, cs);

        globalCs += cs;
        globalVolume += metrics.getVolume();
        globalCyclomaticComplexity += metrics.getCyclomaticComplexity();
        globalLinesOfCode += linesOfCode;
    }

    MetricsCalculator globalMetrics(globalCs, globalLinesOfCode);
    if (verbosity > 0) {
        std::cout << "\nGLOBAL STATISTICS\n";
        globalMetrics.report(verbosity, "Global", globalLinesOfCode, globalCs);
    }

    return EXIT_SUCCESS;
}
