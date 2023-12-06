%{                                                            /* -*- C++ -*- */

#include "parser.hh"
#include "scanner.hh"
#include "codestatistics.hh"

/*  Defines some macros to update locations */
#define STEP()      yylloc->step();
#define COL(Col)    yylloc->columns(Col);
#define LINE(Line)  yylloc->lines(Line);
#define YY_USER_ACTION  COL(yyleng);

int ParserLineno;

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
%x comment
%s includestate

/*
%option stack
*/

/* Abbreviations.  */

blank   		[ \t]+
eol     		[\n\r]+
D			  	[0-9]
L			  	[a-zA-Z_]
H			  	[a-fA-F0-9]
E			  	[Ee][+-]?{D}+
FS				(f|F|l|L)
IS				(u|U|l|L)*
SYCL_parallel_for \w+\.parallel_for<.*?>\(.*?\).*\)|\w+\.parallel_for\(.*?\)

%%

 /* The rules. */
%{
  STEP();
%}


{blank}       STEP();
{eol}         LINE(yyleng);

  /***************** C comments ***************/
						
"/*"                    						{ BEGIN(comment); }
<comment>[^*\n]*        						{ /* eat anything that's not a '*' */ }
<comment>"*"+[^*/\n]*   						{ /* eat up '*'s not followed by '/'s */ }
<comment>\n             						{ ParserLineno++; }
<comment>"*"+"/"        						{ BEGIN(INITIAL); }

  /********* Preprocessor ********************/

\#[\t ]*define          						{ stats.category(SC::KEYWORD, yytext); }
\#[\t ]*else            						{ stats.category(SC::KEYWORD, yytext); }
\#[\t ]*endif           						{ /* nothing*/ }
\#[\t ]*if              						{ stats.category(SC::KEYWORD, yytext); stats.addCondition(); }
\#[\t ]*ifdef           						{ stats.category(SC::KEYWORD, yytext); stats.addCondition(); }
\#[\t ]*ifndef          						{ stats.category(SC::KEYWORD, yytext); stats.addCondition(); }
\#[\t ]*include         						{ stats.category(SC::KEYWORD, yytext); BEGIN(includestate); }
\#[\t ]*pragma          						{ stats.category(SC::KEYWORD, yytext); }
\#[\t ]*line.*          						{ }

<includestate>\<								{ /* do not count as operator */ }
<includestate>\>								{ /* do not count as operator */ }

  /********* Eat Comments and lines **********/

"//".*      									{ /* eat C++ style comments */}

  /***************** Keywords ***************/

int         									{ stats.category(SC::TYPE, yytext); }
float       									{ stats.category(SC::TYPE, yytext); }
char        									{ stats.category(SC::TYPE, yytext); }
double      									{ stats.category(SC::TYPE, yytext); }
long        									{ stats.category(SC::TYPE, yytext); }
short       									{ stats.category(SC::TYPE, yytext); }
signed      									{ stats.category(SC::TYPE, yytext); }
unsigned    									{ stats.category(SC::TYPE, yytext); }
void        									{ stats.category(SC::TYPE, yytext); }
auto        									{ stats.category(SC::CSPECIFIER, yytext); }
extern      									{ stats.category(SC::CSPECIFIER, yytext); }
register    									{ stats.category(SC::CSPECIFIER, yytext); }
static      									{ stats.category(SC::CSPECIFIER, yytext); }
typedef     									{ stats.category(SC::CSPECIFIER, yytext); }
const       									{ stats.category(SC::CSPECIFIER, yytext); }
volatile    									{ stats.category(SC::CSPECIFIER, yytext); }
break       									{ stats.category(SC::KEYWORD, yytext); }
case	    									{ stats.category(SC::KEYWORD, yytext); stats.addCondition(); }
continue    									{ stats.category(SC::KEYWORD, yytext); }
default     									{ stats.category(SC::KEYWORD, yytext); stats.addCondition(); }
do	    										{ stats.category(SC::KEYWORD, yytext); }
else											{ stats.category(SC::KEYWORD, yytext); }
enum											{ stats.category(SC::KEYWORD, yytext); }
for	    										{ stats.category(SC::KEYWORD, yytext); stats.addCondition(); stats.decOperator(); }
goto    										{ stats.category(SC::KEYWORD, yytext); }
if	    										{ stats.category(SC::KEYWORD, yytext); stats.addCondition(); stats.decOperator(); }
return  										{ stats.category(SC::KEYWORD, yytext); }
sizeof  										{ stats.category(SC::KEYWORD, yytext); }
struct  										{ stats.category(SC::KEYWORD, yytext); }
switch  										{ stats.category(SC::KEYWORD, yytext); stats.decOperator(); }
union											{ stats.category(SC::KEYWORD, yytext); }
while											{ stats.category(SC::KEYWORD, yytext); stats.decOperator(); }

  /***************** C+ - specific ***************/
bool        									{ stats.category(SC::TYPE, yytext); }
wchar_t     									{ stats.category(SC::TYPE, yytext); }
false       									{ stats.category(SC::CONSTANT, yytext); }
true        									{ stats.category(SC::CONSTANT, yytext); }
inline      									{ stats.category(SC::CSPECIFIER, yytext); }
mutable     									{ stats.category(SC::CSPECIFIER, yytext); }
virtual     									{ stats.category(SC::CSPECIFIER, yytext); }
catch       									{ stats.category(SC::KEYWORD, yytext); stats.decOperator(); }
class       									{ stats.category(SC::KEYWORD, yytext); }
const_cast  									{ stats.category(SC::KEYWORD, yytext); stats.decOperator(); }
delete      									{ stats.category(SC::KEYWORD, yytext); }
dynamic_cast 									{ stats.category(SC::KEYWORD, yytext); stats.decOperator(); }
explicit    									{ stats.category(SC::KEYWORD, yytext); }
export      									{ stats.category(SC::KEYWORD, yytext); }
friend      									{ stats.category(SC::KEYWORD, yytext); }
namespace   									{ stats.category(SC::KEYWORD, yytext); }
new         									{ stats.category(SC::KEYWORD, yytext); }
operator    									{ stats.category(SC::KEYWORD, yytext); }
private     									{ stats.category(SC::KEYWORD, yytext); }
protected   									{ stats.category(SC::KEYWORD, yytext); }
public      									{ stats.category(SC::KEYWORD, yytext); }
reinterpret_cast 								{ stats.category(SC::KEYWORD, yytext); stats.decOperator(); }
static_cast 									{ stats.category(SC::KEYWORD, yytext); stats.decOperator(); }
template    									{ stats.category(SC::KEYWORD, yytext); stats.decOperator(); }
this        									{ stats.category(SC::KEYWORD, yytext); }
throw       									{ stats.category(SC::KEYWORD, yytext); }
try         									{ stats.category(SC::KEYWORD, yytext); }
typeid      									{ stats.category(SC::KEYWORD, yytext); }
typename    									{ stats.category(SC::KEYWORD, yytext); }
using       									{ stats.category(SC::KEYWORD, yytext); }
asm         									{ printf("asm is unsupported\n"); }

  /***************** C++14, C++17, C++20 - specific ***************/
alignas     									{ stats.category(SC::KEYWORD, yytext); }
alignof     									{ stats.category(SC::KEYWORD, yytext); }
constexpr   									{ stats.category(SC::KEYWORD, yytext); }
decltype    									{ stats.category(SC::KEYWORD, yytext); }
noexcept    									{ stats.category(SC::KEYWORD, yytext); }
nullptr     									{ stats.category(SC::CONSTANT, yytext); }
static_assert 									{ stats.category(SC::KEYWORD, yytext); }
thread_local 									{ stats.category(SC::KEYWORD, yytext); }
concept      									{ stats.category(SC::KEYWORD, yytext); }
co_await     									{ stats.category(SC::KEYWORD, yytext); }
co_return    									{ stats.category(SC::KEYWORD, yytext); }
co_yield     									{ stats.category(SC::KEYWORD, yytext); }
requires     									{ stats.category(SC::KEYWORD, yytext); }
std::get   										{ stats.category(SC::OPERATOR, yytext); }
std::tuple 										{ stats.category(SC::TYPE, yytext); }

  /***************** oneTBB - specific ***************/
"parallel_for"         							{ stats.category(SC::KEYWORD, yytext); stats.addCondition(); }
"parallel_reduce"      							{ stats.category(SC::KEYWORD, yytext); stats.addCondition(); }
"parallel_scan"        							{ stats.category(SC::KEYWORD, yytext); stats.addCondition(); }
"parallel_pipeline"    							{ stats.category(SC::KEYWORD, yytext); stats.addCondition(); }
"flow_graph"           							{ stats.category(SC::KEYWORD, yytext); }
"global_control"       							{ stats.category(SC::KEYWORD, yytext); }
"task_group"           							{ stats.category(SC::KEYWORD, yytext); }
"task_arena"           							{ stats.category(SC::KEYWORD, yytext); }
"blocked_range"        							{ stats.category(SC::TYPE, yytext); }
"blocked_range2d"      							{ stats.category(SC::TYPE, yytext); }
"blocked_range3d"      							{ stats.category(SC::TYPE, yytext); }	
"concurrent_vector"    							{ stats.category(SC::TYPE, yytext); }
"concurrent_queue"     							{ stats.category(SC::TYPE, yytext); }
"concurrent_bounded_queue" 						{ stats.category(SC::TYPE, yytext); }
"concurrent_priority_queue" 					{ stats.category(SC::TYPE, yytext); }
"concurrent_hash_map"  							{ stats.category(SC::TYPE, yytext); }
"concurrent_unordered_map" 						{ stats.category(SC::TYPE, yytext); }
"concurrent_unordered_set" 						{ stats.category(SC::TYPE, yytext); }
"combinable"           							{ stats.category(SC::TYPE, yytext); }
"enumerable_thread_specific" 					{ stats.category(SC::TYPE, yytext); }
"strict_ppl"           							{ stats.category(SC::KEYWORD, yytext); }
"tick_count"           							{ stats.category(SC::TYPE, yytext); }

  /* oneTBB synchronization operators and functions */
"atomic"               							{ stats.category(SC::TYPE, yytext); }
"mutex"                							{ stats.category(SC::TYPE, yytext); }
"spin_mutex"           							{ stats.category(SC::TYPE, yytext); }
"recursive_mutex"      							{ stats.category(SC::TYPE, yytext); }
"queuing_mutex"        							{ stats.category(SC::TYPE, yytext); }
"spin_rw_mutex"        							{ stats.category(SC::TYPE, yytext); }
"queuing_rw_mutex"     							{ stats.category(SC::TYPE, yytext); }
"reader_writer_lock"   							{ stats.category(SC::TYPE, yytext); }

  /* Specific oneTBB functions used for the creation of parallel_pipeline */
"make_filter"          							{ stats.category(SC::KEYWORD, yytext); }
"tbb::filter_mode::parallel"                   	{ stats.category(SC::KEYWORD, yytext); }
"tbb::filter_mode::serial_out_of_order"        	{ stats.category(SC::KEYWORD, yytext); }

  /* Specific oneTBB functions used for the creation of task flow graphs */
"tbb::flow::make_edge"      					{ stats.category(SC::KEYWORD, yytext); }
"tbb::flow::input_port"     					{ stats.category(SC::KEYWORD, yytext); }
"tbb::flow::output_port"    					{ stats.category(SC::KEYWORD, yytext); }
"tbb::flow::remove_edge"    					{ stats.category(SC::KEYWORD, yytext); stats.decOperator(); }
"tbb::flow::continue_node"  					{ stats.category(SC::TYPE, yytext); }
"tbb::flow::function_node"  					{ stats.category(SC::TYPE, yytext); }
"tbb::flow::broadcast_node" 					{ stats.category(SC::TYPE, yytext); }
"tbb::flow::join_node"      					{ stats.category(SC::TYPE, yytext); }
"tbb::flow::split_node"     					{ stats.category(SC::TYPE, yytext); }
"tbb::flow::overwrite_node" 					{ stats.category(SC::TYPE, yytext); }
"tbb::flow::write_once_node" 					{ stats.category(SC::TYPE, yytext); }
"tbb::flow::sequencer_node" 					{ stats.category(SC::TYPE, yytext); }
"tbb::flow::limiter_node"   					{ stats.category(SC::TYPE, yytext); }
"tbb::flow::source_node"            			{ stats.category(SC::TYPE, yytext); }
"tbb::flow::priority_queue_node"    			{ stats.category(SC::TYPE, yytext); }
"tbb::flow::buffer_node"            			{ stats.category(SC::TYPE, yytext); }
"tbb::flow::async_node"             			{ stats.category(SC::TYPE, yytext); }
"tbb::flow::indexer_node"           			{ stats.category(SC::TYPE, yytext); }
"tbb::flow::input_node"             			{ stats.category(SC::TYPE, yytext); }
"tbb::flow::output_node"            			{ stats.category(SC::TYPE, yytext); }
"tbb::flow::multifunction_node"     			{ stats.category(SC::TYPE, yytext); }
"tbb::flow::graph"                  			{ stats.category(SC::TYPE, yytext); }

  /* Specific oneTBB functions for parallelism and task control */
"parallel_invoke"       						{ stats.category(SC::KEYWORD, yytext); stats.addCondition(); }
"parallel_do"           						{ stats.category(SC::KEYWORD, yytext); stats.addCondition(); }
"parallel_sort"         						{ stats.category(SC::KEYWORD, yytext); stats.addCondition(); }
"parallel_for_each"     						{ stats.category(SC::KEYWORD, yytext); stats.addCondition(); }
"enqueue"               						{ stats.category(SC::KEYWORD, yytext); }
"execute"               						{ stats.category(SC::KEYWORD, yytext); }
"wait_for_all"          						{ stats.category(SC::KEYWORD, yytext); }
"reserve_wait"          						{ stats.category(SC::KEYWORD, yytext); }
"try_put"               						{ stats.category(SC::KEYWORD, yytext); }
"release_wait"          						{ stats.category(SC::KEYWORD, yytext); }
"tbb::flow::reserving"  						{ stats.category(SC::KEYWORD, yytext); }
"tbb::flow::unlimited"  						{ stats.category(SC::KEYWORD, yytext); }

  /***************** AVX - specific ***************/
__m256       									{ stats.category(SC::TYPE, yytext); }
__m256d      									{ stats.category(SC::TYPE, yytext); }
__m256i      									{ stats.category(SC::TYPE, yytext); }
_mm256_      									{ stats.category(SC::KEYWORD, yytext); }
_mm256_add_   									{ stats.category(SC::KEYWORD, yytext); }
_mm256_sub_   									{ stats.category(SC::KEYWORD, yytext); }
_mm256_mul_   									{ stats.category(SC::KEYWORD, yytext); }
_mm256_div_   									{ stats.category(SC::KEYWORD, yytext); }
_mm256_and_   									{ stats.category(SC::KEYWORD, yytext); }
_mm256_load_  									{ stats.category(SC::KEYWORD, yytext); }
_mm256_store_ 									{ stats.category(SC::KEYWORD, yytext); }

  /***************** SIMD - specific ***************/
simd_t       									{ stats.category(SC::TYPE, yytext); }
simd_i       									{ stats.category(SC::TYPE, yytext); }
simd_f       									{ stats.category(SC::TYPE, yytext); }
stdx::where   									{ stats.category(SC::KEYWORD, yytext); stats.decOperator(); }
stdx::reduce  									{ stats.category(SC::KEYWORD, yytext); stats.decOperator(); }
element_aligned 								{ stats.category(SC::KEYWORD, yytext); }

  /***************** SYCL - specific ***************/
{SYCL_parallel_for}		{ /* stats.category(SC::KEYWORD, yytext); stats.addCondition(); */ std::cout << "SYCL_parallel_for" << std::endl; }

  /***************** SYCL Types ************************/
"sycl::handler"    								{ stats.category(SC::TYPE, yytext); }
"sycl::id"   									{ stats.category(SC::TYPE, yytext); }
"event"											{ stats.category(SC::TYPE, yytext); }
"sycl::range"									{ stats.category(SC::TYPE, yytext); }
"accessor"										{ stats.category(SC::TYPE, yytext); }
"device"       									{ stats.category(SC::TYPE, yytext); }
"platform"     									{ stats.category(SC::TYPE, yytext); }
"context"      									{ stats.category(SC::TYPE, yytext); }
"nd_range"     									{ stats.category(SC::TYPE, yytext); }
"buffer"       									{ stats.category(SC::TYPE, yytext); }
"sycl::queue"  									{ stats.category(SC::TYPE, yytext); }
"property_list" 								{ stats.category(SC::TYPE, yytext); }
"sycl::backend"									{ stats.category(SC::TYPE, yytext); }
"sycl::property"								{ stats.category(SC::TYPE, yytext); }
"sycl::device"									{ stats.category(SC::TYPE, yytext); }
"sycl::info"									{ stats.category(SC::TYPE, yytext); }


  /***************** SYCL Function Calls ***************/
"sycl::get_nd_range" 							{ stats.category(SC::APIFUNCTION, yytext); }
"get_group_range"								{ stats.category(SC::APIFUNCTION, yytext); }
"get_global_range"								{ stats.category(SC::APIFUNCTION, yytext); }
"get_local_range"								{ stats.category(SC::APIFUNCTION, yytext); }
"get_global_id"									{ stats.category(SC::APIFUNCTION, yytext); }
"get_local_id"     								{ stats.category(SC::APIFUNCTION, yytext); }
"barrier"           							{ stats.category(SC::APIFUNCTION, yytext); }
"submit"            							{ stats.category(SC::APIFUNCTION, yytext); }
"depends_on"									{ stats.category(SC::APIFUNCTION, yytext); }
"sycl::mad"         							{ stats.category(SC::APIFUNCTION, yytext); }
"parallel_for<>"								{ stats.category(SC::APIFUNCTION, yytext); stats.addCondition(); }
"sycl::range<2>"    							{ stats.category(SC::APIFUNCTION, yytext); }
"single_task"       							{ stats.category(SC::APIFUNCTION, yytext); stats.addCondition(); }
"create_sub_devices"  							{ stats.category(SC::APIFUNCTION, yytext); }
"get_device"									{ stats.category(SC::APIFUNCTION, yytext); }
"get_platform"									{ stats.category(SC::APIFUNCTION, yytext); }

  /***************** Operators ***************/
"..."											{ stats.category(SC::OPERATOR, yytext); }
"::"    										{ stats.category(SC::OPERATOR, yytext); }
">>="											{ stats.category(SC::OPERATOR, yytext); }
"<<="											{ stats.category(SC::OPERATOR, yytext); }
"+="											{ stats.category(SC::OPERATOR, yytext); }
"-="											{ stats.category(SC::OPERATOR, yytext); }
"*="											{ stats.category(SC::OPERATOR, yytext); }
"/="											{ stats.category(SC::OPERATOR, yytext); }
"%="											{ stats.category(SC::OPERATOR, yytext); }
"&="											{ stats.category(SC::OPERATOR, yytext); }
"^="											{ stats.category(SC::OPERATOR, yytext); }
"|="											{ stats.category(SC::OPERATOR, yytext); }
">>"											{ stats.category(SC::OPERATOR, yytext); }
"<<"											{ stats.category(SC::OPERATOR, yytext); }
"++"											{ stats.category(SC::OPERATOR, yytext); }
"--"											{ stats.category(SC::OPERATOR, yytext); }
"->"											{ stats.category(SC::OPERATOR, yytext); }
"&&"											{ stats.category(SC::OPERATOR, yytext); }
"||"											{ stats.category(SC::OPERATOR, yytext); }
"<="											{ stats.category(SC::OPERATOR, yytext); }
">="											{ stats.category(SC::OPERATOR, yytext); }
"=="											{ stats.category(SC::OPERATOR, yytext); }
"!="											{ stats.category(SC::OPERATOR, yytext); }
";"												{ stats.category(SC::OPERATOR, yytext); }
("{"|"<%")										{ stats.category(SC::OPERATOR, yytext); }
("}"|"%>")										{ stats.category(SC::OPERATOR, yytext); }
","												{ stats.category(SC::OPERATOR, yytext); }
":"												{            							}
"="												{ stats.category(SC::OPERATOR, yytext); }
"("												{ stats.category(SC::OPERATOR, yytext); }
")"												{            							}
("["|"<:")										{ stats.category(SC::OPERATOR, yytext); }
("]"|":>")										{            							}
"."												{ stats.category(SC::OPERATOR, yytext); }
"&"												{ stats.category(SC::OPERATOR, yytext); }
"!"												{ stats.category(SC::OPERATOR, yytext); }
"~"												{ stats.category(SC::OPERATOR, yytext); }
"-"												{ stats.category(SC::OPERATOR, yytext); }
"+"												{ stats.category(SC::OPERATOR, yytext); }
"*"												{ stats.category(SC::OPERATOR, yytext); }
"/"												{ stats.category(SC::OPERATOR, yytext); }
"%"												{ stats.category(SC::OPERATOR, yytext); }
"<"												{ stats.category(SC::OPERATOR, yytext); }
">"												{ stats.category(SC::OPERATOR, yytext); }
"^"												{ stats.category(SC::OPERATOR, yytext); }
"|"												{ stats.category(SC::OPERATOR, yytext); }
"?"												{ stats.category(SC::OPERATOR, yytext); }



  /***************** Constants ***************/
0[xX]{H}+{IS}?									{ stats.category(SC::CONSTANT, yytext); }
0{D}+{IS}?										{ stats.category(SC::CONSTANT, yytext); }
{D}+{IS}?										{ stats.category(SC::CONSTANT, yytext); }
L?'(\\.|[^\\'])+'								{ stats.category(SC::CONSTANT, yytext); }
{D}+{E}{FS}?									{ stats.category(SC::CONSTANT, yytext); }
{D}*"."{D}+({E})?{FS}?							{ stats.category(SC::CONSTANT, yytext); }
{D}+"."{D}*({E})?{FS}?							{ stats.category(SC::CONSTANT, yytext); }
L?\"(\\.|[^\\"])*\"     						{ stats.category(SC::CONSTANT, yytext); /* STRING_LITERAL*/ }

  /******** Function Calls *********/
[a-zA-Z_][a-zA-Z0-9_]*\s*\(([^)]|\s*\([^\)]*\))*\) {
  printf("function: %s\n", yytext);
}

  /***************** Identifiers ***************/
{L}({L}|{D})*									{ stats.category(SC::IDENTIFIER, yytext); }

  /***************** END OF FILE ***************/
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
