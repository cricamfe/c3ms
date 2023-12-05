%{                                                            /* -*- C++ -*- */

#include "parser.hh"
#include "scanner.hh"
#include "driver.hh"

/*  Defines some macros to update locations */


#define STEP()                                      \
  do {                                              \
    yylloc->step ();                      \
  } while (0)

#define COL(Col)				                            \
  yylloc->columns (Col)

#define LINE(Line)				                          \
  do{						                                    \
    yylloc->lines (Line);		              \
 } while (0)

#define YY_USER_ACTION				                      \
  COL(yyleng);


typedef c3ms::CodeParser::token token;
typedef c3ms::CodeParser::token_type token_type;

#define yyterminate() return token::TOK_EOF

%}

%option debug
%option c++
%option noyywrap
%option never-interactive
%option yylineno
%option nounput
%option batch
%option prefix="c3ms"

/*
%option stack
*/

/* Abbreviations.  */

blank   [ \t]+
eol     [\n\r]+

%%

 /* The rules.  */
%{
  STEP();
%}


{blank}       STEP();

{eol}         LINE(yyleng);

.             {
                std::cerr << *yylloc << " Unexpected token : "
                                              << *yytext << std::endl;
                driver.error_ = (driver.error_ == 127 ? 127
                                : driver.error_ + 1);
                STEP ();
              }

%%


/*

   CUSTOM C++ CODE

*/

namespace c3ms
{

    CodeScanner::CodeScanner() : c3msFlexLexer() {}

    CodeScanner::~CodeScanner() {}

    void CodeScanner::set_debug(bool b) {
        yy_flex_debug = b;
    }
}

#ifdef yylex
# undef yylex
#endif

int c3msFlexLexer::yylex()
{
  std::cerr << "call c3mspitFlexLexer::yylex()!" << std::endl;
  return 0;
}
