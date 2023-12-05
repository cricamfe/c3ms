
#ifndef DRIVER_HH_
# define DRIVER_HH_

# include <string>
# include <iostream>
# include <fstream>

namespace c3ms
{
    /// Forward declarations of classes
    class CodeParser;
    class CodeScanner;
    class location;

    class CodeStatistics
    {
        public:
            CodeStatistics();
            ~CodeStatistics();

            int parse();
            int parse(std::istream &iss);
            int parse_file(std::string& path);

            void reset();

            void addChar(std::size_t n);

            std::ofstream& print(std::ofstream& ofs);

        private:
            CodeScanner* scanner_;
            CodeParser* parser_;
            int error_;

            std::size_t chars = 0;
            std::size_t words = 0;
            std::size_t lines = 0;

            /// Allows Parser and Scanner to access private attributes of the Driver class
            friend class CodeParser;
            friend class CodeScanner;
    };
}

#endif /* !DRIVER_HH_ */

