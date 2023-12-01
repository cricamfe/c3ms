%{
  
  /* Copyright (c) 2009, 2010 Basilio B. Fraguela. Universidade da Coruña */
  
	#include <cassert>
	#include <cstdio>
	#include <cstdlib>
	#include <cstring>
	#include <stdio.h>
	#include <string.h>
	#include <ctype.h>

	#include "CodeStatisticsGatherer.h"
  
	extern int ParserLineno; /* Defined in anl.flex */
	
	int yylex (void);
	void yyerror (char const *);
	
	extern CodeStatisticsGatherer stat;

	void processFunction(char *functionString);
	void processArguments(char *args);

	void trimSpaces(char *str) {
		int start = 0, end = strlen(str) - 1;

		while (str[start] == ' ' || str[start] == '\t') start++;
		while ((end >= start) && (str[end] == ' ' || str[end] == '\t')) end--;

		for (int i = start; i <= end; i++) {
			str[i - start] = str[i];
		}
		str[end - start + 1] = '\0';
	}



	void processFunction(char *functionString) {
		char buffer[1024];
		strcpy(buffer, functionString);

		// Encuentra el primer paréntesis para separar el nombre de la función de los argumentos
		char *parenthesis = strchr(buffer, '(');
		if (parenthesis != NULL) {
			*parenthesis = '\0';  // Corta la cadena para aislar el nombre de la función
			trimSpaces(buffer);
			stat.keyword(buffer);

			// Procesa los argumentos
			processArguments(parenthesis + 1);
		}
	}

	void processArguments(char *args) {
		int nestedFunctionCount = 0;
		char *start = args;
		char arg[1024];

		for (char *p = args; *p; p++) {
			if (*p == '(') {
				nestedFunctionCount++;
			} else if (*p == ')') {
				nestedFunctionCount--;
				if (nestedFunctionCount < 0) { // Fin de los argumentos de la función actual
					strncpy(arg, start, p - start);
					arg[p - start] = '\0';
					trimSpaces(arg);
					stat.identifier(arg);
					return;
				}
			} else if (*p == ',' && nestedFunctionCount == 0) {
				strncpy(arg, start, p - start);
				arg[p - start] = '\0';
				trimSpaces(arg);

				if (strchr(arg, '(')) {
					processFunction(arg);
				} else {
					stat.identifier(arg);
				}
				start = p + 1; // Empieza el siguiente argumento
			}
		}

		// Procesar el último argumento
		if (start) {
			strncpy(arg, start, strlen(start));
			arg[strlen(start)] = '\0';
			trimSpaces(arg);

			if (strchr(arg, '(')) {
				processFunction(arg);
			} else {
				stat.identifier(arg);
			}
		}
	}

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

