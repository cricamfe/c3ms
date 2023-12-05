
#ifndef __C3MSSCANNER_HH__
# define __C3MSSCANNER_HH__

# include "parser.hh"


# ifndef YY_DECL
#  define YY_DECL c3ms::CodeParser::token_type                         \
     c3ms::CodeScanner::yylex(c3ms::CodeParser::semantic_type* yylval,    \
                              c3ms::CodeParser::location_type* yylloc, \
                              c3ms::CodeStatistics& driver)
# endif


# ifndef __FLEX_LEXER_H
#  define yyFlexLexer c3msFlexLexer
#  include <FlexLexer.h>
#  undef yyFlexLexer
# endif


namespace c3ms {
    class CodeScanner : public c3msFlexLexer
    {
        public:
            CodeScanner();

            virtual ~CodeScanner();

            virtual CodeParser::token_type yylex(
                CodeParser::semantic_type* yylval,
                CodeParser::location_type* yylloc,
                CodeStatistics& driver);

            void set_debug(bool b);
    };
}

#endif // __CODESCANNER_HH__
