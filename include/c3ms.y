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

void trimSpaces(char *str);
void processArguments(char *args);
void processFunction(char *functionString);
void extractVariableNames(const char* arg);

void keyword(const char* name) {
    stat.keyword(name);
    // printf("Function: %s\n", name);
}

void identifier(const char* arg) {
    extractVariableNames(arg);
}

void extractVariableNames(const char* arg) {
    char buffer[1024];
    char *tokens[3];
    const char *ampersand = strchr(arg, '&');
    const char *start = ampersand ? ampersand + 1 : arg;
    const char *bracket = strchr(start, '[');
    char *part1, *part2;

    strcpy(buffer, start);
    if (bracket) {
        buffer[bracket - start] = '\0';
        part1 = buffer;
        part2 = buffer + (bracket - start) + 1;
    } else {
        part1 = buffer;
        part2 = NULL;
    }

    char *token = strtok(part1, " ");
    int count = 0;
    while (token != NULL && count < 3) {
        tokens[count++] = token;
        token = strtok(NULL, " ");
    }

    if (count == 3) {
        // printf("Const: %s\n", tokens[0]);
        // printf("Type: %s\n", tokens[1]);
        // printf("Name: %s\n", tokens[2]);
        stat.cspec(tokens[0]);
        stat.type(tokens[1]);
        stat.identifier(tokens[2]);
    } else if (count == 2) {
        // printf("Type: %s\n", tokens[0]);
        // printf("Name: %s\n", tokens[1]);
        stat.type(tokens[0]);
        stat.identifier(tokens[1]);
    } else if (count == 1) {
        // printf("Name: %s\n", tokens[0]);
        stat.identifier(tokens[0]);
    }

    if (part2) {
        char *end;
        token = strtok(part2, "[]");
        while (token != NULL) {
            long val = strtol(token, &end, 10);
            if (end != token) {
                // printf("Number: %s\n", token);
                stat.constant(token);
            } else {
                // printf("Variable: %s\n", token);
                stat.identifier(token);
            }
            token = strtok(NULL, "[]");
        }
    }
}

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
    for (int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] == '\n' || buffer[i] == '\t') {
            buffer[i] = ' ';
        }
    }

    char *openParen = strchr(buffer, '(');
    char *closeParen = strrchr(buffer, ')');

    if (openParen != NULL && closeParen != NULL && openParen < closeParen) {
        char *name = strtok(buffer, "(");
        if (name != NULL) {
            trimSpaces(name);
            keyword(name);
        }

        char args[1024];
        strncpy(args, openParen + 1, closeParen - openParen - 1);
        args[closeParen - openParen - 1] = '\0';

        if (args[0] != '\0') {
            processArguments(args);
        }
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
        } else if (*p == ',' && nestedFunctionCount == 0) {
            strncpy(arg, start, p - start);
            arg[p - start] = '\0';
            trimSpaces(arg);

            if (strchr(arg, '(')) {
                processFunction(arg);
            } else {
                identifier(arg);
            }
            start = p + 1;
        }
    }

    if (start < args + strlen(args)) {
        strncpy(arg, start, args + strlen(args) - start);
        arg[args + strlen(args) - start] = '\0';
        trimSpaces(arg);

        if (strchr(arg, '(')) {
            processFunction(arg);
        } else {
            identifier(arg);
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

