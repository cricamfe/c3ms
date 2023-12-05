%{                                                            /* -*- C++ -*- */

#include "parser.hh"
#include "scanner.hh"
#include "codestatistics.hh"

/*  Defines some macros to update locations */
#define STEP()      yylloc->step();
#define COL(Col)    yylloc->columns(Col);
#define LINE(Line)  yylloc->lines(Line);
#define YY_USER_ACTION  COL(yyleng);

typedef c3ms::CodeParser::token token;
typedef c3ms::CodeParser::token_type token_type;

#define yyterminate() return token::TOK_EOF

// Import enum IDType from CodeStatistics
typedef c3ms::CodeStatistics::StatsCategory SC;

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

 /* The rules. */
%{
  STEP();
%}


{blank}       STEP();
{eol}         LINE(yyleng);
"int" {
    stats.incrementCounter(SC::TYPE, yytext);
}
"float" {
	stats.incrementCounter(SC::TYPE, yytext);
}

.		{
                std::cerr << *yylloc << " Unexpected token : " << *yytext << std::endl;
                stats.error_ = (stats.error_ == 127 ? 127 : stats.error_ + 1);
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
    void CodeScanner::set_debug(bool b) { yy_flex_debug = b; }
}

#ifdef yylex
#undef yylex
#endif

int c3msFlexLexer::yylex()
{
  std::cerr << "call c3mspitFlexLexer::yylex()!" << std::endl;
  return 0;
}
