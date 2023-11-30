/* Copyright 2023 Campos-Ferrer, Cristian. Universidad de Málaga */

#include "CodeUtils.hpp"

// Creates a temporary file with a unique name using the provided baseName
std::string createTemporaryFile(const std::string& functionCode, const std::string& baseName) {
    // Append "XXXXXX" as a placeholder for mkstemps to generate a unique file name
    std::string tempFilename = baseName + "XXXXXX";
    std::vector<char> tempFilenameVector(tempFilename.begin(), tempFilename.end());
    tempFilenameVector.push_back('\0'); // Ensure null-termination for C-style string

    // Create a unique temporary file, returning a file descriptor
    int fd = mkstemps(tempFilenameVector.data(), 0);
    if (fd == -1) {
        // Error handling if file creation fails
        std::cerr << "Error creating temporary file. Template: " << tempFilename << std::endl;
        throw std::runtime_error("Unable to create temporary file");
    }

    // Open a file stream with the file descriptor
    FILE* tempFile = fdopen(fd, "w");
    if (tempFile == nullptr) {
        // Error handling if file stream opening fails
        close(fd);
        std::cerr << "Failed to open file stream for: " << tempFilename << std::endl;
        throw std::runtime_error("Failed to open file stream");
    }

    // Write the function code to the temporary file
    fputs(functionCode.c_str(), tempFile);
    fclose(tempFile); // Close the file stream

    // Return the temporary file name (excluding the added null character)
    return std::string(tempFilenameVector.begin(), tempFilenameVector.end() - 1);
}

// Deletes a temporary file given its filename
void deleteTemporaryFile(const std::string& filename) {
    std::remove(filename.c_str()); // Remove file using C++ standard library function
}

// Prints a formatted header for output sections
void printHeader(const std::string& title, const std::string& color) {
    std::cout << color; // Set the desired color for the header
    std::cout << "\n" << std::string(80, '=') << "\n"; // Print a line of '=' characters
    std::cout << title << "\n"; // Print the title of the header
    std::cout << std::string(80, '=') << "\n"; // Print another line of '=' characters
    std::cout << RESET; // Reset the color to default
}

// Parses command-line arguments and returns a vector of file paths
std::vector<std::filesystem::path> parseArguments(int argc, char* argv[], int& verbosity, bool& functionMetricsFlag, bool& fileMetricsFlag, bool& globalMetricsFlag, bool& printCodeFlag) {
    std::vector<std::filesystem::path> filepaths; // Vector to store parsed file paths

    // Loop through all command-line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i]; // Current argument

        // Check if the argument is verbosity flag and next argument is available
        if ((arg == "-v" || arg == "--verbosity") && i + 1 < argc) {
            verbosity = std::stoi(argv[++i]); // Set verbosity level, converting string to integer
        } else if (arg == "-f" || arg == "--function-metrics") {
            functionMetricsFlag = true; // Enable function metrics analysis
        } else if (arg == "-a" || arg == "--file-metrics") {
            fileMetricsFlag = true; // Enable file metrics analysis
        } else if (arg == "-g" || arg == "--global-metrics") {
            globalMetricsFlag = true; // Enable global metrics analysis
        } else if (arg == "-p" || arg == "--print-functions") {
            printCodeFlag = true; // Enable printing of function contents
        } else if (arg == "-h" || arg == "--help") {
            usage(); // Display usage information and exit
        } else {
            filepaths.emplace_back(argv[i]); // Add file path to the vector
        }
    }

    return filepaths; // Return the vector of parsed file paths
}

// Extracts functions from a given file and returns them as a vector
std::vector<FunctionCode> extractFunctions(const std::string& filePath) {
    std::ifstream file(filePath); // Open the file for reading
    std::string line; // Variable to hold each line of the file
    std::vector<FunctionCode> functions; // Vector to store extracted functions
    std::ostringstream currentFunction; // Stream to build function code
    bool inFunction = false; // Flag to track if currently parsing a function
    int braceCount = 0; // Counter for open braces to detect function blocks

    // Check if the file is successfully opened
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo: " << filePath << std::endl;
        return functions; // Return an empty vector if file can't be opened
    }

    // Regular expressions to match function signatures and names
    std::regex functionPattern(
        R"(([\w\:\s\*\<\>\&\[\]]+\s+[\w\:\s\*\<\>\&\[\]]+\s*\([^)]*\)\s*(const)?\s*(noexcept)?\s*(override)?\s*)[\s\S]*?)"
    );
    std::regex namePattern(R"(\b(\w+)\s*\([^)]*\)\s*\{)");

    // Read the file line by line
    while (std::getline(file, line)) {
        if (inFunction) {
            // Count opening and closing braces
            braceCount += std::count(line.begin(), line.end(), '{');
            braceCount -= std::count(line.begin(), line.end(), '}');

            // Check if the end of a function block is reached
            if (braceCount == 0 && line.find("}") != std::string::npos) {
                inFunction = false; // Reset the flag
                currentFunction << line << "\n"; // Add the closing line to the function code
                std::string funcStr = currentFunction.str(); // Store the function code as a string
                std::smatch nameMatch;
                // Search for the function name using regex
                if (std::regex_search(funcStr, nameMatch, namePattern)) {
                    functions.push_back({nameMatch[1], funcStr}); // Add the function to the vector
                }
                // Clear the stream for the next function
                currentFunction.str("");
                currentFunction.clear();
            }
        } else {
            std::smatch matches;
            // Check for the start of a function block using regex
            if (std::regex_search(line, matches, functionPattern)) {
                inFunction = true; // Set the flag
                braceCount = std::count(line.begin(), line.end(), '{');
                currentFunction.str("");
                currentFunction.clear();
            }
        }

        // If inside a function block, add the line to the function code
        if (inFunction) {
            currentFunction << line << "\n";
        }
    }

    return functions; // Return the vector of extracted functions
}

// Counts and returns the number of lines of code in a file
int countLinesOfCode(const std::string& filePath) {
    std::ifstream file(filePath); // Open the file for reading
    if (!file) {
        // Error handling if file can't be opened
        std::cerr << "Error trying to open file: " << filePath << std::endl;
        return -1; // Return -1 to indicate an error
    }
    // Count and return the number of newline characters in the file
    return std::count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n');
}

// Prints usage information for the application
void usage() {
    // Header
    std::cout << GREEN << "C++ Code Complexity Measurement System" << RESET << "\n\n";

    // Usage
    std::cout << YELLOW << "Usage:" << RESET << " c3ms [-h] [-f] [-a] [-g] [-p DEBUG] [-v level] <files>\n\n";

    // Options
    std::cout << CYAN << "Options:" << RESET << "\n";
    std::cout << "-h, --help                 " << MAGENTA << "Show this help message" << RESET << "\n";
    std::cout << "-f, --function-metrics     " << MAGENTA << "Analyze and report metrics for each function" << RESET << "\n";
    std::cout << "-a, --file-metrics         " << MAGENTA << "Analyze and report metrics for each file" << RESET << "\n";
    std::cout << "-g, --global-metrics       " << MAGENTA << "Analyze and report global metrics across all files" << RESET << "\n";
    std::cout << "-p, --print-functions      " << MAGENTA << "Print the contents of each function (DEBUG)" << RESET << "\n";
    std::cout << "-v, --verbosity [level]    " << MAGENTA << "Set verbosity level (1-3)" << RESET << "\n\n";

    // Verbosity levels
    std::cout << BLUE << "Verbosity levels:" << RESET << "\n";
    std::cout << "    1 - " << GREEN << "Basic metrics (Effort, Volume, Conditions, Cyclomatic Complexity, Difficulty, Time Required, Bugs, Maintainability)" << RESET << "\n";
    std::cout << "    2 - " << GREEN << "Basic metrics and Additional Statistics (Types, Constants, Identifiers, Cspecs, Keywords, Operators)" << RESET << "\n";
    std::cout << "    3 - " << GREEN << "All above metrics and Detailed Metrics (n1 - unique operators, n2 - unique operands, N1 - total operators, N2 - total operands)" << RESET << "\n";

    // Exit the program after displaying usage information
    exit(EXIT_SUCCESS);
}