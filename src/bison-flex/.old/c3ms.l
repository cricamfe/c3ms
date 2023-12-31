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

%option noyywrap

D			  [0-9]
L			  [a-zA-Z_]
H			  [a-fA-F0-9]
E			  [Ee][+-]?{D}+
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

  /***************** C++14, C++17, C++20 - specific ***************/
alignas     { stat.keyword(yytext); }
alignof     { stat.keyword(yytext); }
constexpr   { stat.keyword(yytext); }
decltype    { stat.keyword(yytext); }
noexcept    { stat.keyword(yytext); }
nullptr     { stat.constant(yytext); }
static_assert { stat.keyword(yytext); }
thread_local { stat.keyword(yytext); }
"if"[\t ]+"constexpr" { stat.keyword(yytext); stat.cond(); stat.decOp(); }
concept      { stat.keyword(yytext); }
co_await     { stat.keyword(yytext); }
co_return    { stat.keyword(yytext); }
co_yield     { stat.keyword(yytext); }
requires     { stat.keyword(yytext); }
"std::get"   { stat.op(yytext); }
"std::tuple" { stat.type(yytext); }

  /***************** oneTBB - specific ***************/
"parallel_for"         { stat.keyword(yytext); stat.cond(); }
"parallel_reduce"      { stat.keyword(yytext); stat.cond(); }
"parallel_scan"        { stat.keyword(yytext); stat.cond(); }
"parallel_pipeline"    { stat.keyword(yytext); stat.cond(); }
"flow_graph"           { stat.keyword(yytext); }

"global_control"       { stat.keyword(yytext); }
"task_group"           { stat.keyword(yytext); }
"task_arena"           { stat.keyword(yytext); }
"blocked_range"        { stat.type(yytext); }
"blocked_range2d"      { stat.type(yytext); }
"blocked_range3d"      { stat.type(yytext); }

"concurrent_vector"    { stat.type(yytext); }
"concurrent_queue"     { stat.type(yytext); }
"concurrent_bounded_queue" { stat.type(yytext); }
"concurrent_priority_queue" { stat.type(yytext); }
"concurrent_hash_map"  { stat.type(yytext); }
"concurrent_unordered_map" { stat.type(yytext); }
"concurrent_unordered_set" { stat.type(yytext); }

"combinable"           { stat.type(yytext); }
"enumerable_thread_specific" { stat.type(yytext); }
"strict_ppl"           { stat.keyword(yytext); }
"tick_count"           { stat.type(yytext); }

  /* oneTBB synchronization operators and functions */
"atomic"               { stat.type(yytext); }
"mutex"                { stat.type(yytext); }
"spin_mutex"           { stat.type(yytext); }
"recursive_mutex"      { stat.type(yytext); }
"queuing_mutex"        { stat.type(yytext); }
"spin_rw_mutex"        { stat.type(yytext); }
"queuing_rw_mutex"     { stat.type(yytext); }
"reader_writer_lock"   { stat.type(yytext); }

  /* Specific oneTBB functions used for the creation of parallel_pipeline */
"make_filter"          { stat.keyword(yytext); }
"tbb::filter_mode::parallel"                   { stat.keyword(yytext); }
"tbb::filter_mode::serial_out_of_order"        { stat.keyword(yytext); }

  /* Specific oneTBB functions used for the creation of task flow graphs */
"tbb::flow::make_edge"      { stat.keyword(yytext); }
"tbb::flow::input_port"     { stat.keyword(yytext); }
"tbb::flow::output_port"    { stat.keyword(yytext); }
"tbb::flow::remove_edge"    { stat.keyword(yytext); stat.decOp(); }
"tbb::flow::continue_node"  { stat.type(yytext); }
"tbb::flow::function_node"  { stat.type(yytext); }
"tbb::flow::broadcast_node" { stat.type(yytext); }
"tbb::flow::join_node"      { stat.type(yytext); }
"tbb::flow::split_node"     { stat.type(yytext); }
"tbb::flow::overwrite_node" { stat.type(yytext); }
"tbb::flow::write_once_node" { stat.type(yytext); }
"tbb::flow::sequencer_node" { stat.type(yytext); }
"tbb::flow::limiter_node"   { stat.type(yytext); }
"tbb::flow::source_node"            { stat.type(yytext); }
"tbb::flow::priority_queue_node"    { stat.type(yytext); }
"tbb::flow::buffer_node"            { stat.type(yytext); }
"tbb::flow::async_node"             { stat.type(yytext); }
"tbb::flow::indexer_node"           { stat.type(yytext); }
"tbb::flow::input_node"             { stat.type(yytext); }
"tbb::flow::output_node"            { stat.type(yytext); }
"tbb::flow::multifunction_node"     { stat.type(yytext); }
"tbb::flow::graph"                  { stat.type(yytext); }

  /* Specific oneTBB functions for parallelism and task control */
"parallel_invoke"              { stat.keyword(yytext); stat.cond(); }
"parallel_do"                  { stat.keyword(yytext); stat.cond(); }
"parallel_sort"                { stat.keyword(yytext); stat.cond(); }
"parallel_for_each"            { stat.keyword(yytext); stat.cond(); }
"enqueue"                { stat.keyword(yytext); }
"execute"                { stat.keyword(yytext); }
"wait_for_all"           { stat.keyword(yytext); }
"reserve_wait"                { stat.keyword(yytext); }
"try_put"                     { stat.keyword(yytext); }
"release_wait"                { stat.keyword(yytext); }
"tbb::flow::reserving"       { stat.keyword(yytext); }
"tbb::flow::unlimited"       { stat.keyword(yytext); }

  /***************** SYCL Types ************************/
"sycl::handler"    		{ stat.type(yytext); }
"sycl::id"   			{ stat.type(yytext); }
"event"					{ stat.type(yytext); }
"sycl::range"			{ stat.type(yytext); }
"accessor"				{ stat.type(yytext); }
"device"       			{ stat.type(yytext); }
"platform"     			{ stat.type(yytext); }
"context"      			{ stat.type(yytext); }
"nd_range"     			{ stat.type(yytext); }
"buffer"       			{ stat.type(yytext); }
"sycl::queue"  			{ stat.type(yytext); }
"property_list" 		{ stat.type(yytext); }
"sycl::backend"			{ stat.type(yytext); }
"sycl::property"		{ stat.type(yytext); }
"sycl::device"			{ stat.type(yytext); }
"sycl::info"			{ stat.type(yytext); }


  /***************** SYCL Function Calls ***************/
"sycl::get_nd_range" 	{ stat.apiFunctionCall(yytext); }
"get_group_range"		{ stat.apiFunctionCall(yytext); }
"get_global_range"		{ stat.apiFunctionCall(yytext); }
"get_local_range"		{ stat.apiFunctionCall(yytext); }
"get_global_id"			{ stat.apiFunctionCall(yytext); }
"get_local_id"     		{ stat.apiFunctionCall(yytext); }
"sycl::mad"        		{ stat.apiFunctionCall(yytext); }
"barrier"           	{ stat.apiFunctionCall(yytext); }
"submit"            	{ stat.apiFunctionCall(yytext); }
"depends_on"			{ stat.apiFunctionCall(yytext); }
"sycl::mad"         	{ stat.apiFunctionCall(yytext); }
"parallel_for<>"		{ stat.apiFunctionCall(yytext); stat.cond(); }
"sycl::range<2>"    	{ stat.apiFunctionCall(yytext); }
"single_task"       	{ stat.apiFunctionCall(yytext); stat.cond(); }
"create_sub_devices"  	{ stat.apiFunctionCall(yytext); }
"get_device"			{ stat.apiFunctionCall(yytext); }
"get_platform"			{ stat.apiFunctionCall(yytext); }

  /***************** AVX - specific ***************/
__m256       { stat.type(yytext); }
__m256d      { stat.type(yytext); }
__m256i      { stat.type(yytext); }
_mm256_      { stat.keyword(yytext); }
_mm256_add_   { stat.keyword(yytext); }
_mm256_sub_   { stat.keyword(yytext); }
_mm256_mul_   { stat.keyword(yytext); }
_mm256_div_   { stat.keyword(yytext); }
_mm256_and_   { stat.keyword(yytext); }
_mm256_load_  { stat.keyword(yytext); }
_mm256_store_ { stat.keyword(yytext); }

  /***************** SIMD - specific ***************/
simd_t       { stat.type(yytext); }
simd_i       { stat.type(yytext); }
simd_f       { stat.type(yytext); }
stdx::where   { stat.keyword(yytext); stat.decOp(); }
stdx::reduce  { stat.keyword(yytext); stat.decOp(); }
element_aligned { stat.keyword(yytext); }

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

  /******** Function Calls *********/
[a-zA-Z_][a-zA-Z0-9_]*\s*\(([^)]|\s*\([^\)]*\))*\) {
  printf("function: %s\n", yytext);
  processFunction(yytext);
}

  /***************** identifiers ***************/
{L}({L}|{D})*	{ stat.identifier(yytext); }

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
 
