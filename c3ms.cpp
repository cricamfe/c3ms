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

struct HalsteadMetrics {
	unsigned int n1;
	unsigned int n2;
	unsigned int N1;
	unsigned int N2;
	unsigned int n;
	unsigned int N;

  	double effort;
  	double volume;
  	double difficulty;
  	double timeRequired;
  	double numberOfBugs;
	int conditions;
	int cyclomaticComplexity;
};

void reportHalsteadMetrics(const HalsteadMetrics& metrics) {
	std::cout << "  n1 (unique operators) = " << std::setw(9) << metrics.n1 << '\n';
	std::cout << "  n2 (unique operands)  = " << std::setw(9) << metrics.n2 << '\n';
	std::cout << "  N1 (total # operators)= " << std::setw(9) << metrics.N1 << '\n';
	std::cout << "  N2 (total # operands) = " << std::setw(9) << metrics.N2 << '\n';
}

struct MaintainabilityIndexMetrics {
    double maintainabilityIndex;
};

// Función para generar un separador personalizado
std::string generateSeparator(char character, int length) {
    return std::string(length, character);
}



extern const CodeStatistics& readFile(const char* fname);

namespace {
	const char RCSId[] = "$Date$ $Revision$";
	const char ExecutableType[] = 
	#ifdef NDEBUG
	"Production";
	#else
	"Development";
	#endif
  
	int verbosity = 0;

	void usage()
	{
		std::cout << "C++ Code Complexity Measurement System " << RCSId << " (" << ExecutableType << ")\n";
		std::cout << "Copyright (c) 2009-10 Basilio B. Fraguela. Universidade da Coruña\n";
		std::cout << "Updated by: " << __DATE__ << '\n';
		std::cout << "c3ms [-v level] <files>\n";
		std::cout << "-v level      verbosity level\n";
		std::exit(EXIT_FAILURE);
	}

	void reportStatistics(const CodeStatistics& cs) {
		std::cout << "  Types      : " << std::setw(7) << cs.getTypes() << " (" << std::setw(7) << cs.getUniqueTypes() << " unique)\n";
		std::cout << "  Constants  : " << std::setw(7) << cs.getConstants() << " (" << std::setw(7) << cs.getUniqueConstants() << " unique)\n";
		std::cout << "  Identifiers: " << std::setw(7) << cs.getIdentifiers() << " (" << std::setw(7) << cs.getUniqueIdentifiers() << " unique)\n";
		std::cout << "  Cspecs     : " << std::setw(7) << cs.getCspecs() << " (" << std::setw(7) << cs.getUniqueCspecs() << " unique)\n";
		std::cout << "  Keywords   : " << std::setw(7) << cs.getKeywords() << " (" << std::setw(7) << cs.getUniqueKeywords() << " unique)\n";
		std::cout << "  Operators  : " << std::setw(7) << cs.getOps() << " (" << std::setw(7) << cs.getUniqueOps() << " unique)\n";
		//printf("Conditions : %d\n", cs.getConds());
	}

	int countLinesOfCode(const std::string& filePath) {
		int lines = 0;
		std::string line;
		std::ifstream file(filePath);
		while (std::getline(file, line)) {
			lines++;
		}
		return lines;
	}

	double calculateMaintainabilityIndex(double volume, int cyclomaticComplexity, int linesOfCode) {
		return 171 - 5.2 * log(volume) - 0.23 * cyclomaticComplexity - 16.2 * log(linesOfCode);
	}

	HalsteadMetrics computeHalsteadMetrics(const CodeStatistics& cs, bool verbose) {
		HalsteadMetrics metrics;
		
		metrics.n1 = cs.getUniqueOperators();
		metrics.n2 = cs.getUniqueOperands();
		metrics.N1 = cs.getOperators();
		metrics.N2 = cs.getOperands();
		metrics.n = metrics.n1 + metrics.n2;
		metrics.N = metrics.N1 + metrics.N2;

		metrics.volume = metrics.N * log2((double)metrics.n);
		metrics.difficulty = (metrics.n1 / 2.0) * (metrics.N2 / (double)metrics.n2);
		metrics.effort = metrics.volume * metrics.difficulty;
		metrics.timeRequired = metrics.effort / 18.0;
		metrics.numberOfBugs = pow(metrics.effort, 2.0 / 3.0) / 3000.0;
		metrics.conditions = cs.getConds();
		metrics.cyclomaticComplexity = metrics.conditions + 1;

		return metrics;
	}
  
	std::vector<std::filesystem::path> config(int argc, char* argv[]) {
		std::vector<std::filesystem::path> filepaths;

		for (int i = 1; i < argc; ++i) {
			std::string arg = argv[i];
			if (arg == "-v" && i + 1 < argc) {
				verbosity = std::stoi(argv[++i]);
			} else {
				filepaths.emplace_back(argv[i]);
			}
		}

		if (filepaths.empty()) {
			usage();
		}

		return filepaths;
	}
  
} //end private namespace

/////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {
    // Inicialización de variables globales
	CodeStatistics cs;
    int globalTotalLinesOfCode = 0;

    auto filepaths = config(argc, argv);

    for (const auto& filePath : filepaths) {
        if (!std::filesystem::exists(filePath) || !std::filesystem::is_regular_file(filePath)) {
            std::cerr << "Error accessing or invalid file: " << filePath << '\n';
            continue;
        }

        const std::string filePathStr = filePath.string();
        const CodeStatistics& csfile = readFile(filePathStr.c_str());
        HalsteadMetrics metrics = computeHalsteadMetrics(csfile, verbosity > 2);
        int linesOfCode = countLinesOfCode(filePathStr);

        // Reporte individual por archivo
        if (verbosity != 0) {
            double maintainabilityIndex = calculateMaintainabilityIndex(metrics.volume, metrics.cyclomaticComplexity, linesOfCode);
            printf("%s: (Effort: %10.2lf) (Volume: %10.2lf) (Conditions: %3d) (Cyclomatic Complexity: %3d) (Difficulty: %3.2lf) (Time Required: %5.2lf) (Delivered Bugs: %1.2lf) (Maintainability Index: %3.2lf)\n",
                filePath.filename().string().c_str(), metrics.effort, metrics.volume, metrics.conditions, metrics.cyclomaticComplexity, metrics.difficulty, metrics.timeRequired, metrics.numberOfBugs, maintainabilityIndex
            );
			if (verbosity > 2) { reportHalsteadMetrics(metrics); }
            if (verbosity > 1) { reportStatistics(csfile); }
        }
		// Suma de métricas individuales
		cs += csfile;
		globalTotalLinesOfCode += linesOfCode;
    }

    // Cálculo global de métricas
	HalsteadMetrics globalMetrics = computeHalsteadMetrics(cs, verbosity > 2);
	double globalMaintainabilityIndex = calculateMaintainabilityIndex(globalMetrics.volume, globalMetrics.cyclomaticComplexity, globalTotalLinesOfCode);
	if (verbosity != 0) {
		printf("\nGLOBAL STATISTICS\n");
		printf("%s\n", generateSeparator('-', 20).c_str());  // Línea de separador
		printf("Total Lines of Code: %d\n", globalTotalLinesOfCode);
	}
	printf("Global metrics: (Effort: %10.2lf) (Volume: %10.2lf) (Conditions: %2d) (Cyclomatic Complexity: %2d) (Difficulty: %3.2lf) (Time Required: %5.2lf) (Delivered Bugs: %1.2lf) (Global Maintainability Index: %3.2lf)\n",
		globalMetrics.effort, globalMetrics.volume, globalMetrics.conditions, globalMetrics.cyclomaticComplexity, globalMetrics.difficulty, globalMetrics.timeRequired, globalMetrics.numberOfBugs, globalMaintainabilityIndex
	);

	if (verbosity > 2) { reportHalsteadMetrics(globalMetrics); }
	if (verbosity > 1) { reportStatistics(cs); }

    return EXIT_SUCCESS;
}
