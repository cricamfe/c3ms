%{
  
  /* Copyright (c) 2009, 2010 Basilio B. Fraguela. Universidade da Coruña */

  /* Portions copied from http://www.lysator.liu.se/c/ANSI-C-grammar-l.html */
  
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stack>

#include "CodeStatisticsGatherer.h"
#include "c3ms.tab.hpp"

  
  extern YYSTYPE yylval;
  
  int ParserLineno;
  CodeStatisticsGatherer stat;

  void resetLexer()
  {
    YY_FLUSH_BUFFER;
  }
  
  void resetFileStatistics()
  {
    ParserLineno = 0;
    stat.reset();
  }
  
%}

%option noyywrap

D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*

%x comment
%s includestate

%%

  /***************** C comments ***************/
  
"/*"                    { BEGIN(comment); }
<comment>[^*\n]*        { /* eat anything that's not a '*' */ }
<comment>"*"+[^*/\n]*   { /* eat up '*'s not followed by '/'s */ }
<comment>\n             { ParserLineno++; }
<comment>"*"+"/"        { BEGIN(INITIAL); }

  /********* Preprocessor ********************/

\#[\t ]*define          { stat.keyword(yytext); }
\#[\t ]*else            { stat.keyword(yytext); /*stat.cond();*/ }
\#[\t ]*endif           { /* nothing*/ }
\#[\t ]*if              { stat.keyword(yytext); stat.cond(); }
\#[\t ]*ifdef           { stat.keyword(yytext); stat.cond(); }
\#[\t ]*ifndef          { stat.keyword(yytext); stat.cond(); }
\#[\t ]*include         { stat.keyword(yytext); BEGIN(includestate); }
\#[\t ]*pragma          { stat.keyword(yytext); }
\#[\t ]*line.*          { }

<includestate>\<              { /* do not count as operator */ }
<includestate>\>              { /* do not count as operator */ }
<includestate>[\n\r]          { ParserLineno++; BEGIN(INITIAL); }

  /********* Eat Comments and lines **********/

"//".*      { /* eat C++ style comments */}
[\n\r]      { ParserLineno++; }
[\t ]+

  /***************** Keywords ***************/
  
int         { stat.type(yytext); }
float       { stat.type(yytext); }
char        { stat.type(yytext); }
double      { stat.type(yytext); }
long        { stat.type(yytext); }
short       { stat.type(yytext); }
signed      { stat.type(yytext); }
unsigned    { stat.type(yytext); }
void        { stat.type(yytext); }

auto        { stat.cspec(yytext); }
extern      { stat.cspec(yytext); }
register    { stat.cspec(yytext); }
static      { stat.cspec(yytext); }
typedef     { stat.cspec(yytext); }
  
const       { stat.cspec(yytext); }
volatile    { stat.cspec(yytext); }

break       { stat.keyword(yytext); }
case	    { stat.keyword(yytext); stat.cond(); }
continue    { stat.keyword(yytext); }
default     { stat.keyword(yytext); stat.cond(); }
do	    { stat.keyword(yytext); }
else	    { stat.keyword(yytext); /*stat.cond();*/ }
enum	    { stat.keyword(yytext); }
for	    { stat.keyword(yytext); stat.cond(); stat.decOp(); }
goto        { stat.keyword(yytext); }
if	    { stat.keyword(yytext); stat.cond(); stat.decOp(); }
return      { stat.keyword(yytext); }
sizeof      { stat.keyword(yytext); }
struct      { stat.keyword(yytext); }
switch      { stat.keyword(yytext); /*cond counted in cases+default*/ stat.decOp(); }
union	    { stat.keyword(yytext); }
while	    { stat.keyword(yytext); stat.cond(); stat.decOp(); }

  /***************** C++ - specific ***************/

bool        { stat.type(yytext); }
wchar_t     { stat.type(yytext); }

false       { stat.constant(yytext); }
true        { stat.constant(yytext); }

inline      { stat.cspec(yytext); }
mutable     { stat.cspec(yytext); }
virtual     { stat.cspec(yytext); }

catch       { stat.keyword(yytext); stat.decOp(); }
class       { stat.keyword(yytext); }
const_cast  { stat.keyword(yytext); stat.decOp(); }
delete      { stat.keyword(yytext); }
dynamic_cast { stat.keyword(yytext); stat.decOp(); }
explicit    { stat.keyword(yytext); }
export      { stat.keyword(yytext); }
friend      { stat.keyword(yytext); }
namespace   { stat.keyword(yytext); }
new         { stat.keyword(yytext); }
operator    { stat.keyword(yytext); }
private     { stat.keyword(yytext); }
protected   { stat.keyword(yytext); }
public      { stat.keyword(yytext); }
reinterpret_cast { stat.keyword(yytext); stat.decOp(); }
static_cast { stat.keyword(yytext); stat.decOp(); }
template    { stat.keyword(yytext); stat.decOp(); /* eiter < or > is not an operator */ }
this        { stat.keyword(yytext); }
throw       { stat.keyword(yytext); }
try         { stat.keyword(yytext); }
typeid      { stat.keyword(yytext); }
typename    { stat.keyword(yytext); }
using       { stat.keyword(yytext); }
asm         { printf("asm is unsupported\n"); }
    
{L}({L}|{D})*	{ stat.identifier(yytext); }

  /***************** operators ***************/
    
"..."	{ stat.op(yytext); }
"::"    { stat.op(yytext); }
">>="	{ stat.op(yytext); }
"<<="	{ stat.op(yytext); }
"+="	{ stat.op(yytext); }
"-="	{ stat.op(yytext); }
"*="	{ stat.op(yytext); }
"/="	{ stat.op(yytext); }
"%="	{ stat.op(yytext); }
"&="	{ stat.op(yytext); }
"^="	{ stat.op(yytext); }
"|="	{ stat.op(yytext); }
">>"	{ stat.op(yytext); }
"<<"	{ stat.op(yytext); }
"++"	{ stat.op(yytext); }
"--"	{ stat.op(yytext); }
"->"	{ stat.op(yytext); }
"&&"	{ stat.op(yytext); }
"||"	{ stat.op(yytext); }
"<="	{ stat.op(yytext); }
">="	{ stat.op(yytext); }
"=="	{ stat.op(yytext); }
"!="	{ stat.op(yytext); }
";"		{ stat.op(yytext); }
("{"|"<%")	{ stat.openBracket();  }
("}"|"%>")	{ stat.closeBracket(); }
","		{ stat.op(yytext); }
":"		{            }
"="		{ stat.op(yytext); }
"("		{ stat.op(yytext); }
")"		{            }
("["|"<:")	{ stat.op(yytext); }
("]"|":>")	{            }
"."		{ stat.op(yytext); }
"&"		{ stat.op(yytext); }
"!"		{ stat.op(yytext); }
"~"		{ stat.op(yytext); }
"-"		{ stat.op(yytext); }
"+"		{ stat.op(yytext); }
"*"		{ stat.op(yytext); }
"/"		{ stat.op(yytext); }
"%"		{ stat.op(yytext); }
"<"		{ stat.op(yytext); }
">"		{ stat.op(yytext); }
"^"		{ stat.op(yytext); }
"|"		{ stat.op(yytext); }
"?"		{ stat.op(yytext); }

\\      { /* macro continuation */ }

  /***************** constants ***************/
  
0[xX]{H}+{IS}?		{ stat.constant(yytext); }
0{D}+{IS}?		{ stat.constant(yytext); }
{D}+{IS}?		{ stat.constant(yytext); }
L?'(\\.|[^\\'])+'	{ stat.constant(yytext); }

{D}+{E}{FS}?		{ stat.constant(yytext); }
{D}*"."{D}+({E})?{FS}?	{ stat.constant(yytext); }
{D}+"."{D}*({E})?{FS}?	{ stat.constant(yytext); }

L?\"(\\.|[^\\"])*\"     { stat.constant(yytext); /* STRING_LITERAL*/ }

.                       { printf("bad character: %c\n", *yytext); }
  
%%
 
