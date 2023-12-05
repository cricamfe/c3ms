#include <iostream>
#include <fstream>
#include "bison-flex/codestatistics.hh"

int main(int argc, char* argv[])
{
    c3ms::CodeStatistics stats;

    // Read file from command line argument (using path)
    if (argc > 1)
    {
        std::string path(argv[1]);
        stats.parse_file(path);
    }
    else // Read file from stdin
    {
        stats.parse();
        std::cout << stats.printOperators();
    }
    return 0;
}
