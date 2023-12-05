#include "driver.hh"
#include "parser.hh"
#include "scanner.hh"

namespace c3ms
{
    CodeStatistics::CodeStatistics()
        : scanner_(new CodeScanner()),
          parser_(new CodeParser(*this)),
          error_(0)
    {
    }

    CodeStatistics::~CodeStatistics()
    {
        delete parser_;
        delete scanner_;
    }

    void CodeStatistics::reset()
    {
        error_ = 0;
    }

    int CodeStatistics::parse()
    {
        scanner_->switch_streams(&std::cin, &std::cerr);
        parser_->parse();
        return error_;
    }

    int CodeStatistics::parse(std::istream &iss)
    {
        scanner_->switch_streams(&iss, &std::cerr);
        parser_->parse();
        return error_;
    }

    int CodeStatistics::parse_file(std::string &path)
    {
        std::ifstream s(path.c_str(), std::ifstream::in);
        scanner_->switch_streams(&s, &std::cerr);

        parser_->parse();

        s.close();

        return error_;
    }

    void CodeStatistics::addChar(std::size_t n = 1) {
        chars += n;
    }


    std::ofstream& CodeStatistics::print(std::ofstream& ofs)
    {
        ofs << "chars: " << chars << std::endl;
        ofs << "words: " << words << std::endl;
        ofs << "lines: " << lines << std::endl;
        return ofs;
    }
}
