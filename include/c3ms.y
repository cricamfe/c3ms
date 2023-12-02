%{
  
  /* Copyright (c) 2009, 2010 Basilio B. Fraguela. Universidade da Coruña */
  
	#include <cassert>
	#include <cstdio>
	#include <cstdlib>
	#include <cstring>
	#include <stdio.h>
	#include <string.h>
	#include <ctype.h>
    #include <stdlib.h>

	#include "CodeStatisticsGatherer.h"
  
	extern int ParserLineno; /* Defined in anl.flex */
	
	int yylex (void);
	void yyerror (char const *);
	
	extern CodeStatisticsGatherer stat;
%}

%union {
	char *str;
}

%token TOPLEVELBLOCK

%start program

%%
program : top_level_sequence
        ;

top_level_sequence:
           /* empty */
        | TOPLEVELBLOCK top_level_sequence
        ;

%%

extern FILE *yyin;
extern void resetLexer();
extern void resetFileStatistics();

/* Called by yyparse on error.  */
void yyerror (char const *s)
{
  fprintf (stderr, "Line %d: %s\n", ParserLineno, s);
}

const CodeStatistics& readFile(const char *fname)
{

  yyin = fopen(fname, "r");

  if(yyin == 0) {
    printf("Unable to open file [%s]\n", fname);
    exit(EXIT_FAILURE);
  }

  resetFileStatistics();
  
  try {
    yyparse();
  } catch (...) {
    fclose(yyin);
    printf("Exception thrown while processing %s\n", fname);
    resetLexer();
    exit(EXIT_FAILURE);
  }
  
  fclose(yyin);
  
  return stat;
}

