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
#include "CodeStatistics.h"

struct HalsteadMetrics {
  	double effort;
  	double volume;
  	double difficulty;
  	double timeRequired;
  	double numberOfBugs;
};

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
	bool displayGlobalData = false;

	void usage()
	{
		std::cout << "C++ Code Complexity Measurement System " << RCSId << " (" << ExecutableType << ")\n";
		std::cout << "Copyright (c) 2009-10 Basilio B. Fraguela. Universidade da Coruna\n";
		std::cout << "c3ms [-g] [-v level] <files>\n";
		std::cout << "-g            display global data\n";
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
  

	HalsteadMetrics computeHalsteadMetrics(const CodeStatistics& cs, bool verbose) {
		HalsteadMetrics metrics;
		
		unsigned int n1 = cs.getUniqueOperators();
		unsigned int n2 = cs.getUniqueOperands();
		unsigned int N1 = cs.getOperators();
		unsigned int N2 = cs.getOperands();
		unsigned int n = n1 + n2;
		unsigned int N = N1 + N2;

		metrics.volume = N * log2((double)n);
		metrics.difficulty = (n1 / 2.0) * (N2 / (double)n2);
		metrics.effort = metrics.volume * metrics.difficulty;
		metrics.timeRequired = metrics.effort / 18.0; // Suponiendo que el esfuerzo está en segundos
		metrics.numberOfBugs = metrics.volume / 3000.0;

		// if (verbose) {
		// 	std::cout << "  n1 (unique operators) = " << std::setw(9) << n1 << '\n';
		// 	std::cout << "  n2 (unique operands)  = " << std::setw(9) << n2 << '\n';
		// 	std::cout << "  N1 (total # operators)= " << std::setw(9) << N1 << '\n';
		// 	std::cout << "  N2 (total # operands) = " << std::setw(9) << N2 << '\n';
		// }

		return metrics;
	}
  
	std::vector<std::filesystem::path> config(int argc, char* argv[]) {
		std::vector<std::filesystem::path> filepaths;

		for (int i = 1; i < argc; ++i) {
			std::string arg = argv[i];
			if (arg == "-g") {
				displayGlobalData = true;
			} else if (arg == "-v" && i + 1 < argc) {
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
	CodeStatistics cs;
	double globalEhat = 0.0, globalVolume = 0.0, globalDifficulty = 0.0, globalTimeRequired = 0.0, globalNumberOfBugs = 0.0;
	CodeStatistics::StatSize totConds = 0;

	// Llamada a config actualizada
	auto filepaths = config(argc, argv);

	for (const auto& filePath : filepaths) {
		double localVolume;

		if (!std::filesystem::exists(filePath) || !std::filesystem::is_regular_file(filePath)) {
			std::cerr << "Error accessing or invalid file: " << filePath << '\n';
			continue;
		}

		const std::string filePathStr = filePath.string();
		const CodeStatistics& csfile = readFile(filePathStr.c_str());
        HalsteadMetrics metrics = computeHalsteadMetrics(csfile, verbosity > 2);
		totConds += csfile.getConds();

		// Extract filename from path, and print it
		std::string filename = filePath.filename().string();
		if (verbosity != 0) {
			printf("%s: %10.2lf  (Vol %10.2lf) (%5d conds) (%3.2lf difficulty) (%5.2lf seconds) (%1.2lf bugs)\n", filename.c_str(), metrics.effort, metrics.volume, csfile.getConds(), metrics.difficulty, metrics.timeRequired, metrics.numberOfBugs);
			if (verbosity > 1) {
				reportStatistics(csfile);
			}
		}

		if (displayGlobalData) {
			cs += csfile;
		}

		globalEhat += metrics.effort;
		globalVolume += metrics.volume;
		globalDifficulty += metrics.difficulty;
		globalTimeRequired += metrics.timeRequired;
		globalNumberOfBugs += metrics.numberOfBugs;
	}
 	printf("\n· Total Effort: %10.2lf (Vol %10.2lf) (%3d conds) (%3.2lf difficulty) (%5.2lf seconds) (%1.2lf bugs)\n", globalEhat, globalVolume, totConds, globalDifficulty, globalTimeRequired, globalNumberOfBugs);

	// Total Global (si displayGlobalData es true)
	if (displayGlobalData) {
		if (verbosity > 1) {
			puts("\n· GLOBAL STATISTICS:");
			reportStatistics(cs);
		}

		HalsteadMetrics globalMetrics = computeHalsteadMetrics(cs, verbosity > 2);
		printf("Total Global Effort: %10.2lf (Vol %10.2lf) (%3d conds) (%3.2lf difficulty) (%5.2lf seconds) (%1.2lf bugs)\n", globalMetrics.effort, globalMetrics.volume, totConds, globalMetrics.difficulty, globalMetrics.timeRequired, globalMetrics.numberOfBugs);
	}

	return EXIT_SUCCESS;
}
