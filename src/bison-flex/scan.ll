%{

#include "parser.hh"
#include "scanner.hh"
#include "codestatistics.hh"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <algorithm>
#include <set>
#include <regex>

/*  Defines some macros to update locations */
#define STEP()			yylloc->step();
#define COL(Col)		yylloc->columns(Col);
#define LINE(Line)		yylloc->lines(Line);
#define YY_USER_ACTION	COL(yyleng);

int ParserLineno;

typedef c3ms::CodeParser::token token;
typedef c3ms::CodeParser::token_type token_type;

#define yyterminate() return token::TOK_EOF

// Advice of the use of external class named CodeStatistics
typedef c3ms::CodeStatistics CodeStatistics;

// Import enum IDType from CodeStatistics
typedef c3ms::CodeStatistics::StatsCategory SC;

void trimSpaces(std::string& str);

%}

%option debug
%option c++
%option noyywrap
%option never-interactive
%option yylineno
%option nounput
%option batch
%option prefix="c3ms"
%x comment
%s includestate

/*
%option stack
*/

/* Abbreviations.  */

blank							[ \t]+
eol								[\n\r]+
D								[0-9]
L								[a-zA-Z_]
H								[a-fA-F0-9]
E								[Ee][+-]?{D}+
FS								(f|F|l|L)
IS								(u|U|l|L)*

/* Abbreviations for SYCL */
SYCL_namespace          		sycl::
SYCL_parallel_for				[a-zA-Z_][a-zA-Z0-9_]*\s*\.\s*parallel_for\s*
SYCL_submit             		\.submit\s*\(\s*
SYCL_combined					sycl::[a-zA-Z_][a-zA-Z0-9_]*(\s*<\s*[a-zA-Z0-9_]+(\s*,\s*[a-zA-Z0-9_]+)*\s*>)?(\s*::\s*[a-zA-Z_][a-zA-Z0-9_]*)*(\(\))?


/* Abbreviations for oneTBB */
oneTBB_prefix					((oneapi::tbb::)|(tbb::))?
oneTBB_flow_prefix				{oneTBB_prefix}flow::
oneTBB_filter_mode				{oneTBB_prefix}filter_mode::
oneTBB_parallel_for				parallel_for

%%

 /* The rules. */
%{
	STEP();
%}


{blank}							STEP();
{eol}							LINE(yyleng);

  /***************** C Comment Handling *****************/
						
"/*"											{BEGIN(comment);}
<comment>[^*\n]*								{/* eat anything that's not a '*' */}
<comment>"*"+[^*/\n]*							{/* eat up '*'s not followed by '/'s */}
<comment>\n										{ParserLineno++;}
<comment>"*"+"/"								{BEGIN(INITIAL);}

  /***************** C++ Print Handling (Ignore) *****************/
printf[\t ]*\( {
    // Ignorar todo dentro de los paréntesis de printf hasta llegar al cierre del paréntesis
    int paren_count = 1;
    while (paren_count > 0) {
        char next_char = yyinput();
        if (next_char == '(') {
            ++paren_count;
        } else if (next_char == ')') {
            --paren_count;
        }
    }
}

  /***************** Preprocessor Directives *****************/

"#"[ \t]*"define"                                { stats.category(SC::KEYWORD, "define"); }
"#"[ \t]*"else"                                  { stats.category(SC::KEYWORD, "else"); }
"#"[ \t]*"endif"                                 { stats.category(SC::KEYWORD, "endif"); }
"#"[ \t]*"if"                                    { stats.category(SC::KEYWORD, "if"); stats.addCondition(); }
"#"[ \t]*"ifdef"                                 { stats.category(SC::KEYWORD, "ifdef"); stats.addCondition(); }
"#"[ \t]*"ifndef"                                { stats.category(SC::KEYWORD, "ifndef"); stats.addCondition(); }
"#"[ \t]*"pragma"                                { stats.category(SC::KEYWORD, "pragma"); }
"#"[ \t]*"line".*                                { /* nothing */ }
"#"[ \t]*"include"[ \t]*     					 { stats.category(SC::KEYWORD, "include"); BEGIN(includestate); }
"if constexpr"[\t ]* {
    // Captura el contenido dentro de los paréntesis de if constexpr
    char next_char;
    std::string content;
    int paren_count = 1;
    yyinput(); // Consume el primer '('
    while (paren_count > 0) {
        next_char = yyinput();
        if (next_char == '(') {
            ++paren_count;
        } else if (next_char == ')') {
            --paren_count;
        }
        if (paren_count > 0) {
            content += next_char;
        }
    }
	// Categoriza la keyword y el contenido
	stats.category(SC::KEYWORD, "if constexpr");
    stats.category(SC::CONSTANT, content.c_str());
}



<includestate>[<"]([^>"]+)[>"]	{
    // Extrae el contenido entre < > o " "
    std::string included_file = yytext;
    included_file = included_file.substr(included_file.find_first_of("<\"") + 1);
    included_file = included_file.substr(0, included_file.find_last_of(">\""));
    trimSpaces(included_file);

    // Categoriza el texto extraído y vuelve al estado inicial
    stats.category(SC::CONSTANT, included_file);
    BEGIN(INITIAL);
}

  /***************** C++ Comment Handling *****************/

"//".*											{/* eat C++ style comments */}

  /***************** C and C++ Keywords *****************/

int												{stats.category(SC::TYPE,yytext);}
float											{stats.category(SC::TYPE,yytext);}
char											{stats.category(SC::TYPE,yytext);}
double											{stats.category(SC::TYPE,yytext);}
long											{stats.category(SC::TYPE,yytext);}
short											{stats.category(SC::TYPE,yytext);}
signed											{stats.category(SC::TYPE,yytext);}
unsigned										{stats.category(SC::TYPE,yytext);}
void											{stats.category(SC::TYPE,yytext);}
size_t											{stats.category(SC::TYPE,yytext);}
auto											{stats.category(SC::CSPECIFIER,yytext);}
extern											{stats.category(SC::CSPECIFIER,yytext);}
register										{stats.category(SC::CSPECIFIER,yytext);}
static											{stats.category(SC::CSPECIFIER,yytext);}
typedef											{stats.category(SC::CSPECIFIER,yytext);}
const											{stats.category(SC::CSPECIFIER,yytext);}
volatile										{stats.category(SC::CSPECIFIER,yytext);}
break											{stats.category(SC::KEYWORD,yytext);}
case											{stats.category(SC::KEYWORD,yytext);stats.addCondition();}
continue										{stats.category(SC::KEYWORD,yytext);}
default											{stats.category(SC::KEYWORD,yytext);stats.addCondition();}
do												{stats.category(SC::KEYWORD,yytext);}
else											{stats.category(SC::KEYWORD,yytext);}
enum											{stats.category(SC::KEYWORD,yytext);}
for												{stats.category(SC::KEYWORD,yytext);stats.addCondition();stats.decOperator();}
goto											{stats.category(SC::KEYWORD,yytext);}
if												{stats.category(SC::KEYWORD,yytext);stats.addCondition();stats.decOperator();}
return											{stats.category(SC::KEYWORD,yytext);}
sizeof											{stats.category(SC::KEYWORD,yytext);}
malloc											{stats.category(SC::KEYWORD,yytext);}
free											{stats.category(SC::KEYWORD,yytext);}
empty                       					{stats.category(SC::KEYWORD,yytext); /* Vector or Array */}
struct											{stats.category(SC::KEYWORD,yytext);}
switch											{stats.category(SC::KEYWORD,yytext);stats.decOperator();}
union											{stats.category(SC::KEYWORD,yytext);}
while											{stats.category(SC::KEYWORD,yytext);stats.decOperator();}
max												{stats.category(SC::KEYWORD,yytext);}
min												{stats.category(SC::KEYWORD,yytext);}
main											{stats.category(SC::KEYWORD,yytext);}

  /***************** C++ Math Functions *****************/
fabs											{stats.category(SC::KEYWORD,yytext);}
abs												{stats.category(SC::KEYWORD,yytext);}
acos											{stats.category(SC::KEYWORD,yytext);}
asin											{stats.category(SC::KEYWORD,yytext);}
atan											{stats.category(SC::KEYWORD,yytext);}
atan2											{stats.category(SC::KEYWORD,yytext);}
ceil											{stats.category(SC::KEYWORD,yytext);}
cos												{stats.category(SC::KEYWORD,yytext);}
cosh											{stats.category(SC::KEYWORD,yytext);}
exp												{stats.category(SC::KEYWORD,yytext);}
floor											{stats.category(SC::KEYWORD,yytext);}
fmod											{stats.category(SC::KEYWORD,yytext);}
frexp											{stats.category(SC::KEYWORD,yytext);}
ldexp											{stats.category(SC::KEYWORD,yytext);}
log												{stats.category(SC::KEYWORD,yytext);}
log2											{stats.category(SC::KEYWORD,yytext);}
log10											{stats.category(SC::KEYWORD,yytext);}
modf											{stats.category(SC::KEYWORD,yytext);}
pow												{stats.category(SC::KEYWORD,yytext);}
sin												{stats.category(SC::KEYWORD,yytext);}
sinh											{stats.category(SC::KEYWORD,yytext);}
sqrt											{stats.category(SC::KEYWORD,yytext);}
tan												{stats.category(SC::KEYWORD,yytext);}
tanh											{stats.category(SC::KEYWORD,yytext);}
acosf											{stats.category(SC::KEYWORD,yytext);}
asinf											{stats.category(SC::KEYWORD,yytext);}
atanf											{stats.category(SC::KEYWORD,yytext);}
atan2f											{stats.category(SC::KEYWORD,yytext);}
cosf											{stats.category(SC::KEYWORD,yytext);}
coshf											{stats.category(SC::KEYWORD,yytext);}
expf											{stats.category(SC::KEYWORD,yytext);}
expm1											{stats.category(SC::KEYWORD,yytext);}
expm1f											{stats.category(SC::KEYWORD,yytext);}
fabsf											{stats.category(SC::KEYWORD,yytext);}
fma												{stats.category(SC::KEYWORD,yytext);}
fmaf											{stats.category(SC::KEYWORD,yytext);}
hypot											{stats.category(SC::KEYWORD,yytext);}
hypotf											{stats.category(SC::KEYWORD,yytext);}
logf											{stats.category(SC::KEYWORD,yytext);}
log1p											{stats.category(SC::KEYWORD,yytext);}
log1pf											{stats.category(SC::KEYWORD,yytext);}
logb											{stats.category(SC::KEYWORD,yytext);}
logbf											{stats.category(SC::KEYWORD,yytext);}
nextafter										{stats.category(SC::KEYWORD,yytext);}
nextafterf										{stats.category(SC::KEYWORD,yytext);}
powf											{stats.category(SC::KEYWORD,yytext);}
remainder										{stats.category(SC::KEYWORD,yytext);}
remainderf										{stats.category(SC::KEYWORD,yytext);}
rint											{stats.category(SC::KEYWORD,yytext);}
rintf											{stats.category(SC::KEYWORD,yytext);}
sinf											{stats.category(SC::KEYWORD,yytext);}
sinhf											{stats.category(SC::KEYWORD,yytext);}
sqrtf											{stats.category(SC::KEYWORD,yytext);}
tanf											{stats.category(SC::KEYWORD,yytext);}
tanhf											{stats.category(SC::KEYWORD,yytext);}
trunc											{stats.category(SC::KEYWORD,yytext);}
truncf											{stats.category(SC::KEYWORD,yytext);}
fpclassify										{stats.category(SC::CONSTANT,yytext);}
isfinite										{stats.category(SC::CONSTANT,yytext);}
isinf											{stats.category(SC::CONSTANT,yytext);}
isnan											{stats.category(SC::CONSTANT,yytext);}
isnormal										{stats.category(SC::CONSTANT,yytext);}
signbit											{stats.category(SC::CONSTANT,yytext);}
isgreater										{stats.category(SC::CONSTANT,yytext);}
isgreaterequal									{stats.category(SC::CONSTANT,yytext);}
isless											{stats.category(SC::CONSTANT,yytext);}
islessequal										{stats.category(SC::CONSTANT,yytext);}
islessgreater									{stats.category(SC::CONSTANT,yytext);}
isunordered										{stats.category(SC::CONSTANT,yytext);}
math_errhandling								{stats.category(SC::CONSTANT,yytext);}
INFINITY										{stats.category(SC::CONSTANT,yytext);}
NAN												{stats.category(SC::CONSTANT,yytext);}
HUGE_VAL										{stats.category(SC::CONSTANT,yytext);}
HUGE_VALF										{stats.category(SC::CONSTANT,yytext);}
HUGE_VALL										{stats.category(SC::CONSTANT,yytext);}

  /***************** C++ Specific Keywords and Types *****************/

bool											{stats.category(SC::TYPE,yytext);}
wchar_t											{stats.category(SC::TYPE,yytext);}
false											{stats.category(SC::CONSTANT,yytext);}
true											{stats.category(SC::CONSTANT,yytext);}
inline											{stats.category(SC::CSPECIFIER,yytext);}
mutable											{stats.category(SC::CSPECIFIER,yytext);}
virtual											{stats.category(SC::CSPECIFIER,yytext);}
catch											{stats.category(SC::KEYWORD,yytext);stats.decOperator();}
class											{stats.category(SC::KEYWORD,yytext);}
const_cast										{stats.category(SC::KEYWORD,yytext);stats.decOperator();}
delete											{stats.category(SC::KEYWORD,yytext);}
dynamic_cast									{stats.category(SC::KEYWORD,yytext);stats.decOperator();}
explicit										{stats.category(SC::KEYWORD,yytext);}
export											{stats.category(SC::KEYWORD,yytext);}
friend											{stats.category(SC::KEYWORD,yytext);}
namespace										{stats.category(SC::KEYWORD,yytext);}
new												{stats.category(SC::KEYWORD,yytext);}
operator										{stats.category(SC::KEYWORD,yytext);}
private											{stats.category(SC::KEYWORD,yytext);}
protected										{stats.category(SC::KEYWORD,yytext);}
public											{stats.category(SC::KEYWORD,yytext);}
reinterpret_cast								{stats.category(SC::KEYWORD,yytext);stats.decOperator();}
static_cast										{stats.category(SC::KEYWORD,yytext);stats.decOperator();}
template										{stats.category(SC::KEYWORD,yytext);stats.decOperator();}
this											{stats.category(SC::KEYWORD,yytext);}
throw											{stats.category(SC::KEYWORD,yytext);}
try												{stats.category(SC::KEYWORD,yytext);}
typeid											{stats.category(SC::KEYWORD,yytext);}
typename										{stats.category(SC::KEYWORD,yytext);}
using											{stats.category(SC::KEYWORD,yytext);}
asm												{printf("asmisunsupported\n");}
vector    										{stats.category(SC::TYPE,yytext);}
"template"[\t ]*"<"[\t ]*[a-zA-Z_][a-zA-Z0-9_]*[\t ]+[a-zA-Z_][a-zA-Z0-9_]*[\t ]*">" {
    char *token;
    // Add the keyword "template"
	stats.category(SC::KEYWORD, "template");
    token = strtok(yytext, " \t<>");
	// Generics type, for example "typename" or "size_t"
    token = strtok(NULL, " \t<>");
    if (token) {
        stats.category(SC::TYPE, token);
    }
	// Add the parameter name
    token = strtok(NULL, " \t<>");
    if (token) {
        stats.category(SC::IDENTIFIER, token);
    }
}

  /***************** C++14, C++17, C++20 Specific Features *****************/
alignas											{stats.category(SC::KEYWORD,yytext);}
alignof											{stats.category(SC::KEYWORD,yytext);}
constexpr										{stats.category(SC::KEYWORD,yytext);}
decltype										{stats.category(SC::KEYWORD,yytext);}
noexcept										{stats.category(SC::KEYWORD,yytext);}
nullptr											{stats.category(SC::CONSTANT,yytext);}
static_assert									{stats.category(SC::KEYWORD,yytext);}
thread_local									{stats.category(SC::KEYWORD,yytext);}
concept											{stats.category(SC::KEYWORD,yytext);}
co_await										{stats.category(SC::KEYWORD,yytext);}
co_return										{stats.category(SC::KEYWORD,yytext);}
co_yield										{stats.category(SC::KEYWORD,yytext);}
requires										{stats.category(SC::KEYWORD,yytext);}
get												{stats.category(SC::KEYWORD,yytext);}
tuple											{stats.category(SC::TYPE,yytext);}
pair											{stats.category(SC::TYPE,yytext);}
std::[a-zA-Z_][a-zA-Z0-9_]*\s*\( 				{
	// Remove the open parenthesis
	std::string buffer = yytext;
	buffer.erase(buffer.size() - 1);
	stats.category(SC::KEYWORD, buffer);
	stats.category(SC::OPERATOR, "(");
}
std::[a-zA-Z_][a-zA-Z0-9_]* 					{stats.category(SC::TYPE,yytext);}	

  /***************** oneTBB Specific Keywords and Types *****************/
{oneTBB_prefix}									{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_parallel_for}							{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_prefix}parallel_reduce					{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_prefix}parallel_scan					{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_prefix}parallel_pipeline				{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_prefix}flow_graph						{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_prefix}task_group						{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_prefix}task_arena						{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_prefix}blocked_range					{stats.category(SC::TYPE,yytext);}
{oneTBB_prefix}blocked_range2d					{stats.category(SC::TYPE,yytext);}
{oneTBB_prefix}blocked_range3d					{stats.category(SC::TYPE,yytext);}	
{oneTBB_prefix}concurrent_vector				{stats.category(SC::TYPE,yytext);}
{oneTBB_prefix}concurrent_queue					{stats.category(SC::TYPE,yytext);}
{oneTBB_prefix}concurrent_bounded_queue			{stats.category(SC::TYPE,yytext);}
{oneTBB_prefix}concurrent_priority_queue		{stats.category(SC::TYPE,yytext);}
{oneTBB_prefix}concurrent_hash_map				{stats.category(SC::TYPE,yytext);}
{oneTBB_prefix}concurrent_unordered_map			{stats.category(SC::TYPE,yytext);}
{oneTBB_prefix}concurrent_unordered_set			{stats.category(SC::TYPE,yytext);}
{oneTBB_prefix}combinable						{stats.category(SC::TYPE,yytext);}
{oneTBB_prefix}enumerable_thread_specific		{stats.category(SC::TYPE,yytext);}
{oneTBB_prefix}global_control					{stats.category(SC::TYPE,yytext);}
{oneTBB_prefix}global_control::max_allowed_parallelism	{stats.category(SC::CONSTANT,yytext);}

  /***************** oneTBB Synchronization and Functions *****************/
{oneTBB_prefix}spin_mutex						{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_prefix}recursive_mutex					{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_prefix}queuing_mutex					{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_prefix}spin_rw_mutex					{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_prefix}queuing_rw_mutex					{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_prefix}reader_writer_lock				{stats.category(SC::APIKEYWORD,yytext);}

  /***************** oneTBB Parallel Pipeline and Task Graph Creation *****************/
{oneTBB_prefix}make_filter						{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_filter_mode}parallel					{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_filter_mode}serial_out_of_order			{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_filter_mode}serial_in_order				{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_flow_prefix}make_edge					{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_flow_prefix}input_port					{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_flow_prefix}output_port					{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_flow_prefix}remove_edge					{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_flow_prefix}continue_node				{stats.category(SC::TYPE,yytext);}
{oneTBB_flow_prefix}function_node				{stats.category(SC::TYPE,yytext);}
{oneTBB_flow_prefix}broadcast_node				{stats.category(SC::TYPE,yytext);}
{oneTBB_flow_prefix}join_node					{stats.category(SC::TYPE,yytext);}
{oneTBB_flow_prefix}split_node					{stats.category(SC::TYPE,yytext);}
{oneTBB_flow_prefix}overwrite_node				{stats.category(SC::TYPE,yytext);}
{oneTBB_flow_prefix}write_once_node				{stats.category(SC::TYPE,yytext);}
{oneTBB_flow_prefix}sequencer_node				{stats.category(SC::TYPE,yytext);}
{oneTBB_flow_prefix}limiter_node				{stats.category(SC::TYPE,yytext);}
{oneTBB_flow_prefix}source_node					{stats.category(SC::TYPE,yytext);}
{oneTBB_flow_prefix}priority_queue_node			{stats.category(SC::TYPE,yytext);}
{oneTBB_flow_prefix}buffer_node					{stats.category(SC::TYPE,yytext);}
{oneTBB_flow_prefix}async_node					{stats.category(SC::TYPE,yytext);}
{oneTBB_flow_prefix}indexer_node				{stats.category(SC::TYPE,yytext);}
{oneTBB_flow_prefix}input_node					{stats.category(SC::TYPE,yytext);}
{oneTBB_flow_prefix}output_node					{stats.category(SC::TYPE,yytext);}
{oneTBB_flow_prefix}multifunction_node			{stats.category(SC::TYPE,yytext);}
{oneTBB_flow_prefix}graph						{stats.category(SC::TYPE,yytext);}
flow_control									{stats.category(SC::TYPE,yytext);}

  /****************** oneTBB Parallel Algorithms *****************/
parallel_invoke									{stats.category(SC::APIKEYWORD,yytext);}
parallel_do										{stats.category(SC::APIKEYWORD,yytext);}
parallel_sort									{stats.category(SC::APIKEYWORD,yytext);}
parallel_for_each								{stats.category(SC::APIKEYWORD,yytext);}
enqueue											{stats.category(SC::APIKEYWORD,yytext);}
execute											{stats.category(SC::APIKEYWORD,yytext);}
wait_for_all									{stats.category(SC::APIKEYWORD,yytext);}
reserve_wait									{stats.category(SC::APIKEYWORD,yytext);}
try_put											{stats.category(SC::APIKEYWORD,yytext);}
stop											{stats.category(SC::APIKEYWORD,yytext);}
release_wait									{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_prefix}reserving						{stats.category(SC::APIKEYWORD,yytext);}
{oneTBB_prefix}unlimited						{stats.category(SC::APIKEYWORD,yytext);}

  /***************** oneTBB Tick Count *****************/
{oneTBB_prefix}tick_count						{stats.category(SC::TYPE,yytext);}
{oneTBB_prefix}tick_count::now					{stats.category(SC::APIKEYWORD,yytext);}

  /***************** AVX Specific Types and Functions *****************/
__m(128|256|512)[di]?							{stats.category(SC::TYPE,yytext);}
_mm(128|256|512)?_[a-zA-Z_][a-zA-Z0-9_]*\s*		{
	// Remove the open parenthesis
	std::string buffer = yytext;
	buffer.erase(buffer.size() - 1);
	stats.category(SC::APILLKEYWORD, buffer);
}

  /***************** SIMD Specific Types and Functions *****************/
simd_t											{stats.category(SC::TYPE,yytext);}
simd_i											{stats.category(SC::TYPE,yytext);}
simd_f											{stats.category(SC::TYPE,yytext);}
stdx::where										{stats.category(SC::APIKEYWORD,yytext);}
stdx::reduce									{stats.category(SC::APIKEYWORD,yytext);}
element_aligned									{stats.category(SC::APIKEYWORD,yytext);}
stdx::[a-zA-Z_][a-zA-Z0-9_]*\s*\( 				{
	// Remove the open parenthesis
	std::string buffer = yytext;
	buffer.erase(buffer.size() - 1);
	stats.category(SC::APIKEYWORD, buffer);
	stats.category(SC::OPERATOR, "(");
}

  /***************** SYCL Specific Types, Keywords and Functions *****************/
{SYCL_parallel_for}								{
	// Vamos a separar por una parte h o como se llame, del parallel_for o single_task
	// Ejemplo: h.parallel_for, h.single_task -> h, parallel_for y h, single_task
	std::string buffer = yytext;
	size_t dot = buffer.find('.');
	std::string h = buffer.substr(0, dot);
	std::string function = buffer.substr(dot + 1);
	stats.category(SC::IDENTIFIER, h);
	stats.category(SC::APIKEYWORD, function);
	stats.addCondition();
}

{SYCL_combined} {
	char next_char = yyinput();
    std::string sycl_combined = yytext;

    // Comprueba si es una función (presencia de '()')
    if (next_char == '(') {
        // Maneja como función
        std::string function_name = sycl_combined.substr(0, sycl_combined.find("("));
        stats.category(SC::APIKEYWORD, function_name);
		stats.category(SC::OPERATOR, "(");
    } else {
        // Maneja como tipo
        size_t openBracket = sycl_combined.find('<');
        size_t closeBracket = sycl_combined.find('>');

        if (openBracket != std::string::npos && closeBracket != std::string::npos && openBracket < closeBracket) {
            // Extraer el nombre del tipo y el contenido dentro de <>
            std::string type_name = sycl_combined.substr(0, openBracket);
            std::string type_param = sycl_combined.substr(openBracket + 1, closeBracket - openBracket - 1);
            trimSpaces(type_param);

            // Añadir el tipo
            stats.category(SC::TYPE, type_name);

            // Comprobar si el parámetro es un número o un identificador
            if (std::all_of(type_param.begin(), type_param.end(), ::isdigit)) {
                stats.category(SC::CONSTANT, type_param);
            } else {
                stats.category(SC::TYPE, type_param);
            }
        } else {
            // Solo es un tipo sin parámetros <>
            stats.category(SC::TYPE, sycl_combined);
        }
		unput(next_char);
    }
}

{SYCL_submit} {
    // Extraer el texto entre el punto y el paréntesis
    std::string buffer = yytext;
    size_t dot = buffer.find('.');
    size_t openParen = buffer.find('(');
    std::string function = buffer.substr(dot + 1, openParen - dot - 1);
    stats.category(SC::APIKEYWORD, function);
    stats.category(SC::OPERATOR, "(");
}

  /***************** SYCL Types *****************/
handler											{stats.category(SC::TYPE,yytext);}
id												{stats.category(SC::TYPE,yytext);}
event											{stats.category(SC::TYPE,yytext);}
range											{stats.category(SC::TYPE,yytext);}
accessor										{stats.category(SC::TYPE,yytext);}
device											{stats.category(SC::TYPE,yytext);}
platform										{stats.category(SC::TYPE,yytext);}
context											{stats.category(SC::TYPE,yytext);}
nd_range										{stats.category(SC::TYPE,yytext);}
buffer											{stats.category(SC::TYPE,yytext);}
queue											{stats.category(SC::TYPE,yytext);}
property_list									{stats.category(SC::TYPE,yytext);}
backend											{stats.category(SC::TYPE,yytext);}
property										{stats.category(SC::TYPE,yytext);}
device											{stats.category(SC::TYPE,yytext);}
info											{stats.category(SC::TYPE,yytext);}
local_accessor									{stats.category(SC::TYPE,yytext);}

  /***************** SYCL Function Calls ***************/
get_nd_range									{stats.category(SC::APIKEYWORD,yytext);}
get_group_range									{stats.category(SC::APIKEYWORD,yytext);}
get_global_range								{stats.category(SC::APIKEYWORD,yytext);}
get_local_range									{stats.category(SC::APIKEYWORD,yytext);}
get_global_id									{stats.category(SC::APIKEYWORD,yytext);}
get_local_id									{stats.category(SC::APIKEYWORD,yytext);}
barrier											{stats.category(SC::APIKEYWORD,yytext);}
depends_on										{stats.category(SC::APIKEYWORD,yytext);}
mad												{stats.category(SC::APIKEYWORD,yytext);}
single_task										{stats.category(SC::APIKEYWORD,yytext);}
create_sub_devices								{stats.category(SC::APIKEYWORD,yytext);}
get_device										{stats.category(SC::APIKEYWORD,yytext);}
get_platform									{stats.category(SC::APIKEYWORD,yytext);}
wait											{stats.category(SC::APIKEYWORD,yytext);}

  /***************** Operator Handling *****************/
"..."											{stats.category(SC::OPERATOR,yytext);}
"::"											{stats.category(SC::OPERATOR,yytext);}
">>="											{stats.category(SC::OPERATOR,yytext);}
"<<="											{stats.category(SC::OPERATOR,yytext);}
"+="											{stats.category(SC::OPERATOR,yytext);}
"-="											{stats.category(SC::OPERATOR,yytext);}
"*="											{stats.category(SC::OPERATOR,yytext);}
"/="											{stats.category(SC::OPERATOR,yytext);}
"%="											{stats.category(SC::OPERATOR,yytext);}
"&="											{stats.category(SC::OPERATOR,yytext);}
"^="											{stats.category(SC::OPERATOR,yytext);}
"|="											{stats.category(SC::OPERATOR,yytext);}
">>"											{stats.category(SC::OPERATOR,yytext);}
"<<"											{stats.category(SC::OPERATOR,yytext);}
"++"											{stats.category(SC::OPERATOR,yytext);}
"--"											{stats.category(SC::OPERATOR,yytext);}
"->"											{stats.category(SC::OPERATOR,yytext);}
"&&"											{stats.category(SC::OPERATOR,yytext);}
"||"											{stats.category(SC::OPERATOR,yytext);}
"<="											{stats.category(SC::OPERATOR,yytext);}
">="											{stats.category(SC::OPERATOR,yytext);}
"=="											{stats.category(SC::OPERATOR,yytext);}
"!="											{stats.category(SC::OPERATOR,yytext);}
";"												{stats.category(SC::OPERATOR,yytext);}
("{"|"<%")										{stats.category(SC::OPERATOR,yytext);}
("}"|"%>")										{stats.category(SC::OPERATOR,yytext);}
","												{stats.category(SC::OPERATOR,yytext);}
":"												{							}
"="												{stats.category(SC::OPERATOR,yytext);}
"("												{stats.category(SC::OPERATOR,yytext);}
")"												{							}
("["|"<:")										{stats.category(SC::OPERATOR,yytext);}
("]"|":>")										{							}
"."												{stats.category(SC::OPERATOR,yytext);}
"&"												{stats.category(SC::OPERATOR,yytext);}
"!"												{stats.category(SC::OPERATOR,yytext);}
"~"												{stats.category(SC::OPERATOR,yytext);}
"-"												{stats.category(SC::OPERATOR,yytext);}
"+"												{stats.category(SC::OPERATOR,yytext);}
"*"												{stats.category(SC::OPERATOR,yytext);}
"/"												{stats.category(SC::OPERATOR,yytext);}
"%"												{stats.category(SC::OPERATOR,yytext);}
"<"												{stats.category(SC::OPERATOR,yytext);}
">"												{stats.category(SC::OPERATOR,yytext);}
"^"												{stats.category(SC::OPERATOR,yytext);}
"|"												{stats.category(SC::OPERATOR,yytext);}
"?"												{stats.category(SC::OPERATOR,yytext);}

  /***************** Numeric and String Constants *****************/
0[xX]{H}+{IS}?									{stats.category(SC::CONSTANT,yytext);}
0{D}+{IS}?										{stats.category(SC::CONSTANT,yytext);}
{D}+{IS}?										{stats.category(SC::CONSTANT,yytext);}
L?'(\\.|[^\\'])+'								{stats.category(SC::CONSTANT,yytext);}
{D}+{E}{FS}?									{stats.category(SC::CONSTANT,yytext);}
{D}*"."{D}+({E})?{FS}?							{stats.category(SC::CONSTANT,yytext);}
{D}+"."{D}*({E})?{FS}?							{stats.category(SC::CONSTANT,yytext);}
L?\"(\\.|[^\\"])*\"								{stats.category(SC::CONSTANT,yytext);/*STRING_LITERAL*/}

  /***************** Identifier Handling *****************/
{L}({L}|{D})*									{
	char next_char = yyinput();
	std::string buffer = yytext;
	if (next_char == '(') {
		// Es una función
		stats.category(SC::CUSTOMKEYWORD,buffer.c_str());
	} else {
		// Es un identificador
		stats.category(SC::IDENTIFIER,yytext);
		unput(next_char);
	}
}

  /***************** End of File Handling and Error Reporting *****************/
.	{
	std::cerr << *yylloc << " Unexpected token : " << *yytext << std::endl;
  int currentError = stats.getError();
  stats.setError(currentError == 127 ? 127 : currentError + 1);
	STEP ();
}

%%

void trimSpaces(std::string& str) {
	str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
		return !std::isspace(ch);
	}));
	str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
	}).base(), str.end());
}

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
