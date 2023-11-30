/* Copyright 2023 Campos-Ferrer, Cristian. Universidad de Málaga */

#ifndef CODE_UTILS_HPP
#define CODE_UTILS_HPP

#include <string>
#include <vector>
#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <fstream>
#include <unistd.h>
#include <regex>

// ANSI color codes
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";
const std::string RESET = "\033[0m";

/**
 * @struct FunctionCode
 * 
 * @brief A struct that holds the name and code of a function.
 * 
 * @details This struct is used to store the name and code of a function extracted from a source code file. 
 * It has two members: a string for the function name and a string for the function code.
 * 
 * @member name The name of the function.
 * @member code The code of the function.
 */
struct FunctionCode {
    std::string name, code;
};

/**
 * @brief Creates a temporary file with the given function code and base name.
 * 
 * @param functionCode The code to be written to the temporary file.
 * @param baseName The base name to be used for the temporary file.
 * @return std::string The name of the temporary file.
 * 
 * @details This function creates a temporary file in the /tmp directory with a name based on the given base name. 
 * It writes the given function code to the temporary file. 
 * If it fails to create the file or open a file stream for writing, it throws a runtime error. 
 * It returns the name of the temporary file.
 */
std::string createTemporaryFile(const std::string& functionCode, const std::string& baseName);

/**
 * @brief Deletes a temporary file.
 * 
 * @param filename The name of the temporary file to be deleted.
 * 
 * @details This function deletes a temporary file with the given name. 
 * It uses the remove function from the standard library, which deletes the file if it exists. 
 * If the file does not exist or cannot be deleted, the function does not report any error.
 */
void deleteTemporaryFile(const std::string& filename);

/**
 * @brief Prints a formatted header with a title and color.
 * 
 * @param title The title to be printed in the header.
 * @param color The color to be used for the header text.
 * 
 * @details This function prints a formatted header to the console. 
 * The header consists of a line of '=' characters, the title, and another line of '=' characters. 
 * The color of the text is set to the specified color before printing the header, and is reset afterwards.
 */
void printHeader(const std::string& title, const std::string& color);

/**
 * @brief Parses command-line arguments and returns a vector of file paths.
 * 
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @param verbosity The verbosity level to be set based on the arguments.
 * @param functionMetricsFlag A flag to be set if function-level metrics are requested.
 * @param fileMetricsFlag A flag to be set if file-level metrics are requested.
 * @param globalMetricsFlag A flag to be set if global metrics are requested.
 * @return std::vector<std::filesystem::path> A vector of file paths to be analyzed.
 * 
 * @details This function parses the command-line arguments provided to the program. 
 * It recognizes the following options: '-v' or '--verbosity' followed by a number to set the verbosity level, 
 * '-f' or '--function-metrics' to request function-level metrics, 
 * '-a' or '--file-metrics' to request file-level metrics, 
 * '-g' or '--global-metrics' to request global metrics, 
 * and '-h' or '--help' to display usage information. 
 * Any other arguments are treated as file paths to be analyzed. 
 * The function returns a vector of these file paths.
 */
std::vector<std::filesystem::path> parseArguments(int argc, char* argv[], int& verbosity, bool& functionMetricsFlag, bool& fileMetricsFlag, bool& globalMetricsFlag);

/**
 * @brief Extracts all functions from a given source code file.
 * 
 * @param filePath Path of the source code file.
 * @return std::vector<FunctionCode> Vector of pairs, where the first element is the function name and the second is the function code.
 * 
 * @details This function opens the specified source code file, reads the file line by line, and uses regular expressions to identify function definitions. 
 * When it finds a function definition, it extracts the function code until it finds the corresponding closing brace. 
 * It then stores the function name and function code in a pair and adds the pair to the function vector. 
 * Finally, it returns the function vector.
 */
std::vector<FunctionCode> extractFunctions(const std::string& filePath);

/**
 * @brief Counts the number of lines of code in a file.
 * 
 * @param filePath The path of the file to be analyzed.
 * @return int The number of lines of code in the file.
 * 
 * @details This function opens a file with the given path and counts the number of newline characters, 
 * which is equivalent to the number of lines in the file. 
 * If the file cannot be opened, it prints an error message and returns -1.
 */
int countLinesOfCode(const std::string& filePath);

/**
 * @brief Prints usage information and exits the program.
 * 
 * @details This function is called when the program is run with the '-h' or '--help' option, 
 * or when the program is run with invalid options. 
 * It prints a message explaining how to use the program, including the available options and their meanings, 
 * and then exits the program with a failure status.
 */
void usage();

#endif // CODE_UTILS_HPP