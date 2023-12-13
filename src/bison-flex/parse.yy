%{     /* PARSER */

#include "parser.hh"
#include "scanner.hh"

#define yylex stats.scanner_->yylex
%}

%code requires
{
  #include <iostream>
  #include "codestatistics.hh"
  #include "location.hh"
}

%code provides
{
  namespace c3ms
  {
    // Forward declaration of the Driver class
    class CodeStatistics;

    inline void
    yyerror (const char* msg)
    {
      std::cerr << msg << std::endl;
    }
  }
}


%skeleton "lalr1.cc"
%require "3.8"
%language "C++"
%locations
%defines
%debug
%define api.namespace {c3ms}
%define api.parser.class {CodeParser}
%parse-param {CodeStatistics &stats}
%lex-param {CodeStatistics &stats}
%define parse.error verbose

%union
{
 /* YYLTYPE */
}

/* Tokens */
%token TOK_EOF 0

/* Entry point of grammar */
%start start

%%

start:
    /* empty */
;



%%

namespace c3ms
{
    void CodeParser::error(const location& l, const std::string& m)
    {
        std::cerr << l << ": " << m << std::endl;
        int currentError = stats.getError();
  		stats.setError(currentError == 127 ? 127 : currentError + 1);
    }
}
