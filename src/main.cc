#include <iostream>
#include <fstream>
#include "bison-flex/codestatistics.hh"

int main(int argc, char* argv[])
{
    c3ms::CodeStatistics driver;

    // Read file from command line argument (using path)
    if (argc > 1)
    {
        std::string path(argv[1]);
        driver.parse_file(path);
        std::ofstream outputFile("output.txt");
        driver.print(outputFile);
    }
    return 0;
}
