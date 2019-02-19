/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is include which follows the "include" declaration
** in the input file. */
#include <stdio.h>
#line 1 "parse.y"

#include "parse.h"
#include "dbval.h"
#include "sql_part.h"
#include "sql_parser.h"

#line 15 "parse.c"
/* Next is all token values, in a form suitable for use by makeheaders.
** This section will be null unless lemon is run with the -m switch.
*/
/* 
** These constants (all generated automatically by the parser generator)
** specify the various kinds of tokens (terminals) that the parser
** understands. 
**
** Each symbol here is a terminal symbol in the grammar.
*/
/* Make sure the INTERFACE macro is defined.
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/* The next thing included is series of defines which control
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 terminals
**                       and nonterminals.  "int" is used otherwise.
**    YYNOCODE           is a number of type YYCODETYPE which corresponds
**                       to no legal terminal or nonterminal number.  This
**                       number is used to fill in empty slots of the hash 
**                       table.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       have fall-back values which should be used if the
**                       original value of the token will not parse.
**    YYACTIONTYPE       is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 rules and
**                       states combined.  "int" is used otherwise.
**    mydbParseTOKENTYPE     is the data type used for minor tokens given 
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is mydbParseTOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.
**    mydbParseARG_SDECL     A static variable declaration for the %extra_argument
**    mydbParseARG_PDECL     A parameter declaration for the %extra_argument
**    mydbParseARG_STORE     Code to store %extra_argument into yypParser
**    mydbParseARG_FETCH     Code to extract %extra_argument from yypParser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
/*  */
#define YYCODETYPE unsigned char
#define YYNOCODE 67
#define YYACTIONTYPE unsigned char
#define mydbParseTOKENTYPE Token
typedef union {
  mydbParseTOKENTYPE yy0;
  ExprItem* yy1;
  GroupOpt* yy3;
  ExprList* yy58;
  ColumnItem* yy68;
  Token yy78;
  ColumnList* yy125;
  LimitOpt* yy126;
  int yy133;
} YYMINORTYPE;
#define YYSTACKDEPTH 100
#define mydbParseARG_SDECL SQLParser *pParse;
#define mydbParseARG_PDECL ,SQLParser *pParse
#define mydbParseARG_FETCH SQLParser *pParse = yypParser->pParse
#define mydbParseARG_STORE yypParser->pParse = pParse
#define YYNSTATE 122
#define YYNRULE 56
#define YYERRORSYMBOL 50
#define YYERRSYMDT yy133
#define YY_NO_ACTION      (YYNSTATE+YYNRULE+2)
#define YY_ACCEPT_ACTION  (YYNSTATE+YYNRULE+1)
#define YY_ERROR_ACTION   (YYNSTATE+YYNRULE)

/* Next are that tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N < YYNSTATE                  Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   YYNSTATE <= N < YYNSTATE+YYNRULE   Reduce by rule N-YYNSTATE.
**
**   N == YYNSTATE+YYNRULE              A syntax error has occurred.
**
**   N == YYNSTATE+YYNRULE+1            The parser accepts its input.
**
**   N == YYNSTATE+YYNRULE+2            No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.  
**
** The formula above is for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
*/
static YYACTIONTYPE yy_action[] = {
 /*     0 */    52,   54,   48,   50,   35,   46,   56,   58,   34,   98,
 /*    10 */     1,   21,   22,   23,   59,   61,    8,  109,   12,   71,
 /*    20 */    73,   67,   69,   63,   65,   26,   76,   43,   37,   93,
 /*    30 */    62,   40,  108,   81,   37,  112,   85,   90,   75,  113,
 /*    40 */    87,   44,   38,   39,   42,   41,   62,   45,   38,   39,
 /*    50 */    42,  111,  117,   45,   32,   84,   89,  114,  153,  148,
 /*    60 */    16,   25,  152,   17,   94,  119,   24,   19,  121,    2,
 /*    70 */   102,    4,  115,  125,   20,  107,   99,   97,   31,   96,
 /*    80 */   104,   95,   18,   31,  100,  137,   91,   88,  110,  101,
 /*    90 */    28,  132,   15,   29,   30,   86,  131,  103,   13,   14,
 /*   100 */   154,   33,    6,  124,    5,  105,   11,   36,   83,   80,
 /*   110 */   106,   92,   79,   82,   10,   78,   74,   72,   70,   77,
 /*   120 */    68,    9,   66,   64,  122,  120,    7,  118,   47,   57,
 /*   130 */     3,  123,  116,  179,   27,  124,   49,  124,   55,  124,
 /*   140 */    51,   53,  124,   60,
};
static YYCODETYPE yy_lookahead[] = {
 /*     0 */    32,   33,   34,   35,   36,   37,   38,   39,   10,   61,
 /*    10 */     5,   16,   17,   18,   40,   41,   11,   10,   13,   32,
 /*    20 */    33,   34,   35,   36,   37,   20,   21,   30,   30,   24,
 /*    30 */    57,   30,   25,   15,   30,   15,   15,   19,   65,   19,
 /*    40 */    19,   44,   44,   45,   46,   44,   57,   49,   44,   45,
 /*    50 */    46,   63,   64,   49,   65,   56,   57,   10,    7,    7,
 /*    60 */    52,   53,    7,   15,   58,    9,   53,   19,   62,    6,
 /*    70 */    19,    8,   25,    0,   10,   19,    7,   60,   26,   54,
 /*    80 */    27,   59,    7,   26,   29,    0,   10,   57,   14,   30,
 /*    90 */    10,    0,   14,   54,    7,    7,    0,   30,   12,   10,
 /*   100 */     7,   31,   10,    0,    9,   28,    7,   57,   14,   55,
 /*   110 */    10,   10,   14,   23,   10,   10,   10,   10,   10,   22,
 /*   120 */    10,   12,   10,   10,    0,   10,    7,   62,   57,   57,
 /*   130 */     7,    0,   64,   51,    9,   66,   57,   66,   57,   66,
 /*   140 */    57,   57,   66,   41,
};
#define YY_SHIFT_USE_DFLT (-33)
static short yy_shift_ofst[] = {
 /*     0 */     5,   63,  123,  124,   95,   92,  119,  131,  109,  104,
 /*    10 */    99,  103,   86,   89,   78,   64,   48,   75,   73,   64,
 /*    20 */    -5,  -33,  -33,  -33,  -33,  -33,  125,   80,   52,   87,
 /*    30 */    96,   -2,   70,   -2,  -32,    4,  -33,  -33,  -33,    1,
 /*    40 */   -33,  -33,   -3,  -33,  -33,  -33,    4,  -33,    4,  -33,
 /*    50 */     4,  -33,    4,  -33,    4,  -33,    4,  -33,  -26,  102,
 /*    60 */   -33,  -33,  -13,  113,  -33,  112,  -33,  110,  -33,  108,
 /*    70 */   -33,  107,  -33,  106,  -33,  -33,   97,  105,   98,  101,
 /*    80 */    18,   90,   94,    4,   21,   88,   91,    4,  -33,  -33,
 /*    90 */    76,  -33,  -33,    7,   56,   57,   53,   55,   69,   85,
 /*   100 */    59,   51,   67,   93,   77,  100,  -33,    7,  -33,   74,
 /*   110 */    47,   20,  -33,   47,  -33,  -33,  -33,  -33,  -33,  115,
 /*   120 */   -33,  -33,
};
#define YY_REDUCE_USE_DFLT (-53)
static signed char yy_reduce_ofst[] = {
 /*     0 */    82,  -53,  -53,  -53,  -53,  -53,  -53,  -53,  -53,  -53,
 /*    10 */   -53,  -53,  -53,  -53,  -53,    8,  -53,  -53,  -53,   13,
 /*    20 */   -53,  -53,  -53,  -53,  -53,  -53,  -53,  -53,   39,  -53,
 /*    30 */   -53,  -11,  -53,  -27,  -53,   50,  -53,  -53,  -53,  -53,
 /*    40 */   -53,  -53,  -53,  -53,  -53,  -53,   71,  -53,   79,  -53,
 /*    50 */    83,  -53,   84,  -53,   81,  -53,   72,  -53,  -53,  -53,
 /*    60 */   -53,  -53,  -53,  -53,  -53,  -53,  -53,  -53,  -53,  -53,
 /*    70 */   -53,  -53,  -53,  -53,  -53,  -53,  -53,  -53,  -53,   54,
 /*    80 */   -53,  -53,  -53,   -1,  -53,  -53,  -53,   30,  -53,  -53,
 /*    90 */   -53,  -53,  -53,    6,   22,   25,   17,  -52,  -53,  -53,
 /*   100 */   -53,  -53,  -53,  -53,  -53,  -53,  -53,   65,  -53,  -53,
 /*   110 */   -12,  -53,  -53,   68,  -53,  -53,  -53,  -53,  -53,  -53,
 /*   120 */   -53,  -53,
};
static YYACTIONTYPE yy_default[] = {
 /*     0 */   178,  178,  178,  178,  178,  178,  178,  178,  178,  178,
 /*    10 */   178,  178,  178,  178,  178,  178,  178,  178,  178,  178,
 /*    20 */   178,  126,  127,  128,  130,  129,  178,  178,  178,  178,
 /*    30 */   178,  178,  149,  178,  178,  178,  155,  171,  172,  178,
 /*    40 */   173,  174,  178,  175,  176,  177,  178,  156,  178,  157,
 /*    50 */   178,  158,  178,  159,  178,  160,  178,  161,  178,  178,
 /*    60 */   162,  163,  178,  178,  164,  178,  165,  178,  166,  178,
 /*    70 */   167,  178,  168,  178,  169,  170,  178,  178,  178,  178,
 /*    80 */   178,  178,  178,  178,  178,  178,  178,  178,  136,  135,
 /*    90 */   178,  134,  133,  178,  178,  148,  150,  178,  178,  178,
 /*   100 */   178,  178,  178,  178,  178,  178,  151,  178,  138,  139,
 /*   110 */   178,  178,  140,  178,  143,  144,  146,  145,  142,  178,
 /*   120 */   147,  141,
};
#define YY_SZ_ACTTAB (sizeof(yy_action)/sizeof(yy_action[0]))

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammer, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
*/
struct yyStackEntry {
  int stateno;       /* The state-number */
  int major;         /* The major token value.  This is the code
                     ** number for the token at this stack level */
  YYMINORTYPE minor; /* The user-supplied minor token value.  This
                     ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
  int yyerrcnt;                 /* Shifts left before out of the error */
  mydbParseARG_SDECL                /* A place to hold %extra_argument */
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void mydbParseTrace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *yyTokenName[] = { 
  "$",             "FUNC",          "ILLEGAL",       "SPACE",       
  "COMMENT",       "SHOW",          "TABLES",        "SEMI",        
  "COLUMNS",       "FROM",          "ID",            "DROP",        
  "TABLE",         "CREATE",        "LP",            "RP",          
  "INT_TYPE",      "DOUBLE_TYPE",   "STRING_TYPE",   "COMMA",       
  "DELETE",        "INSERT",        "INTO",          "VALUES",      
  "SELECT",        "STAR",          "WHERE",         "GROUP",       
  "BY",            "LIMIT",         "INTEGER",       "AND",         
  "EQ",            "NE",            "GT",            "GE",          
  "LT",            "LE",            "LIKE",          "IS",          
  "NOT",           "NULL",          "ISNOTNULL",     "ISNULL",      
  "DOUBLE",        "PLUS",          "MINUS",         "UINTEGER",    
  "UDOUBLE",       "STRING",        "error",         "cmd",         
  "col_list",      "col_item",      "where_opt",     "name_list",   
  "val_list",      "val_item",      "target_list",   "from",        
  "groupby_opt",   "limit_opt",     "target_item",   "arg_list",    
  "arg_item",      "condi_expr",  
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *yyRuleName[] = {
 /*   0 */ "cmd ::= SHOW TABLES SEMI",
 /*   1 */ "cmd ::= SHOW COLUMNS FROM ID SEMI",
 /*   2 */ "cmd ::= DROP TABLE ID SEMI",
 /*   3 */ "cmd ::= CREATE TABLE ID LP col_list RP SEMI",
 /*   4 */ "col_item ::= ID INT_TYPE",
 /*   5 */ "col_item ::= ID DOUBLE_TYPE",
 /*   6 */ "col_item ::= ID STRING_TYPE",
 /*   7 */ "col_list ::= col_item",
 /*   8 */ "col_list ::= col_list COMMA col_item",
 /*   9 */ "cmd ::= DELETE FROM ID where_opt SEMI",
 /*  10 */ "cmd ::= INSERT INTO ID LP name_list RP VALUES LP val_list RP SEMI",
 /*  11 */ "name_list ::= ID",
 /*  12 */ "name_list ::= name_list COMMA ID",
 /*  13 */ "val_list ::= val_item",
 /*  14 */ "val_list ::= val_list COMMA val_item",
 /*  15 */ "cmd ::= SELECT target_list from where_opt groupby_opt limit_opt SEMI",
 /*  16 */ "target_item ::= STAR",
 /*  17 */ "target_item ::= ID",
 /*  18 */ "target_item ::= ID LP arg_list RP",
 /*  19 */ "target_list ::= target_item",
 /*  20 */ "target_list ::= target_list COMMA target_item",
 /*  21 */ "arg_item ::= ID",
 /*  22 */ "arg_item ::= STAR",
 /*  23 */ "arg_list ::= arg_item",
 /*  24 */ "arg_list ::= arg_list COMMA arg_item",
 /*  25 */ "from ::= FROM ID",
 /*  26 */ "where_opt ::=",
 /*  27 */ "where_opt ::= WHERE condi_expr",
 /*  28 */ "groupby_opt ::=",
 /*  29 */ "groupby_opt ::= GROUP BY ID",
 /*  30 */ "limit_opt ::=",
 /*  31 */ "limit_opt ::= LIMIT INTEGER",
 /*  32 */ "limit_opt ::= LIMIT INTEGER COMMA INTEGER",
 /*  33 */ "condi_expr ::= ID LT val_item",
 /*  34 */ "condi_expr ::= ID LE val_item",
 /*  35 */ "condi_expr ::= ID GT val_item",
 /*  36 */ "condi_expr ::= ID GE val_item",
 /*  37 */ "condi_expr ::= ID EQ val_item",
 /*  38 */ "condi_expr ::= ID NE val_item",
 /*  39 */ "condi_expr ::= ID LIKE val_item",
 /*  40 */ "condi_expr ::= ID IS NOT NULL",
 /*  41 */ "condi_expr ::= ID IS NULL",
 /*  42 */ "condi_expr ::= val_item LT ID",
 /*  43 */ "condi_expr ::= val_item LE ID",
 /*  44 */ "condi_expr ::= val_item GT ID",
 /*  45 */ "condi_expr ::= val_item GE ID",
 /*  46 */ "condi_expr ::= val_item EQ ID",
 /*  47 */ "condi_expr ::= val_item NE ID",
 /*  48 */ "condi_expr ::= condi_expr AND condi_expr",
 /*  49 */ "val_item ::= INTEGER",
 /*  50 */ "val_item ::= DOUBLE",
 /*  51 */ "val_item ::= PLUS INTEGER",
 /*  52 */ "val_item ::= PLUS DOUBLE",
 /*  53 */ "val_item ::= MINUS INTEGER",
 /*  54 */ "val_item ::= MINUS DOUBLE",
 /*  55 */ "val_item ::= STRING",
};
#endif /* NDEBUG */

/*
** This function returns the symbolic name associated with a token
** value.
*/
const char *mydbParseTokenName(int tokenType){
#ifndef NDEBUG
  if( tokenType>0 && tokenType<(sizeof(yyTokenName)/sizeof(yyTokenName[0])) ){
    return yyTokenName[tokenType];
  }else{
    return "Unknown";
  }
#else
  return "";
#endif
}

/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to mydbParse and mydbParseFree.
*/
void *mydbParseAlloc(void *(*mallocProc)(size_t)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (size_t)sizeof(yyParser) );
  if( pParser ){
    pParser->yyidx = -1;
  }
  return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
*/
static void yy_destructor(YYCODETYPE yymajor, YYMINORTYPE *yypminor){
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are not used
    ** inside the C code.
    */
    case 52:
#line 42 "parse.y"
{ delete ((yypminor->yy125)); }
#line 444 "parse.c"
      break;
    case 53:
#line 40 "parse.y"
{ delete ((yypminor->yy68)); }
#line 449 "parse.c"
      break;
    case 54:
#line 131 "parse.y"
{ delete ((yypminor->yy1)); }
#line 454 "parse.c"
      break;
    case 55:
#line 67 "parse.y"
{ delete ((yypminor->yy58)); }
#line 459 "parse.c"
      break;
    case 56:
#line 78 "parse.y"
{ delete ((yypminor->yy58)); }
#line 464 "parse.c"
      break;
    case 57:
#line 178 "parse.y"
{ delete ((yypminor->yy1)); }
#line 469 "parse.c"
      break;
    case 58:
#line 95 "parse.y"
{ delete ((yypminor->yy58)); }
#line 474 "parse.c"
      break;
    case 60:
#line 137 "parse.y"
{ delete ((yypminor->yy3)); }
#line 479 "parse.c"
      break;
    case 61:
#line 143 "parse.y"
{ delete ((yypminor->yy126)); }
#line 484 "parse.c"
      break;
    case 62:
#line 97 "parse.y"
{ delete ((yypminor->yy1)); }
#line 489 "parse.c"
      break;
    case 63:
#line 114 "parse.y"
{ delete ((yypminor->yy58)); }
#line 494 "parse.c"
      break;
    case 64:
#line 112 "parse.y"
{ delete ((yypminor->yy1)); }
#line 499 "parse.c"
      break;
    case 65:
#line 156 "parse.y"
{ delete ((yypminor->yy1)); }
#line 504 "parse.c"
      break;
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
**
** Return the major token number for the symbol popped.
*/
static int yy_pop_parser_stack(yyParser *pParser){
  YYCODETYPE yymajor;
  yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

  if( pParser->yyidx<0 ) return 0;
#ifndef NDEBUG
  if( yyTraceFILE && pParser->yyidx>=0 ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yymajor = yytos->major;
  yy_destructor( yymajor, &yytos->minor);
  pParser->yyidx--;
  return yymajor;
}

/* 
** Deallocate and destroy a parser.  Destructors are all called for
** all stack elements before shutting the parser down.
**
** Inputs:
** <ul>
** <li>  A pointer to the parser.  This should be a pointer
**       obtained from mydbParseAlloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void mydbParseFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
  if( pParser==0 ) return;
  while( pParser->yyidx>=0 ) yy_pop_parser_stack(pParser);
  (*freeProc)((void*)pParser);
}

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  int iLookAhead            /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  /* if( pParser->yyidx<0 ) return YY_NO_ACTION;  */
  i = yy_shift_ofst[stateno];
  if( i==YY_SHIFT_USE_DFLT ){
    return yy_default[stateno];
  }
  if( iLookAhead==YYNOCODE ){
    return YY_NO_ACTION;
  }
  i += iLookAhead;
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
#ifdef YYFALLBACK
    int iFallback;            /* Fallback token */
    if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
           && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
           yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
      }
#endif
      return yy_find_shift_action(pParser, iFallback);
    }
#endif
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_reduce_action(
  yyParser *pParser,        /* The parser */
  int iLookAhead            /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  i = yy_reduce_ofst[stateno];
  if( i==YY_REDUCE_USE_DFLT ){
    return yy_default[stateno];
  }
  if( iLookAhead==YYNOCODE ){
    return YY_NO_ACTION;
  }
  i += iLookAhead;
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  YYMINORTYPE *yypMinor         /* Pointer ot the minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yyidx++;
  if( yypParser->yyidx>=YYSTACKDEPTH ){
     mydbParseARG_FETCH;
     yypParser->yyidx--;
#ifndef NDEBUG
     if( yyTraceFILE ){
       fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
     }
#endif
     while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
     /* Here code is inserted which will execute if the parser
     ** stack every overflows */
     mydbParseARG_STORE; /* Suppress warning about unused %extra_argument var */
     return;
  }
  yytos = &yypParser->yystack[yypParser->yyidx];
  yytos->stateno = yyNewState;
  yytos->major = yyMajor;
  yytos->minor = *yypMinor;
#ifndef NDEBUG
  if( yyTraceFILE && yypParser->yyidx>0 ){
    int i;
    fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
    fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
    for(i=1; i<=yypParser->yyidx; i++)
      fprintf(yyTraceFILE," %s",yyTokenName[yypParser->yystack[i].major]);
    fprintf(yyTraceFILE,"\n");
  }
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 51, 3 },
  { 51, 5 },
  { 51, 4 },
  { 51, 7 },
  { 53, 2 },
  { 53, 2 },
  { 53, 2 },
  { 52, 1 },
  { 52, 3 },
  { 51, 5 },
  { 51, 11 },
  { 55, 1 },
  { 55, 3 },
  { 56, 1 },
  { 56, 3 },
  { 51, 7 },
  { 62, 1 },
  { 62, 1 },
  { 62, 4 },
  { 58, 1 },
  { 58, 3 },
  { 64, 1 },
  { 64, 1 },
  { 63, 1 },
  { 63, 3 },
  { 59, 2 },
  { 54, 0 },
  { 54, 2 },
  { 60, 0 },
  { 60, 3 },
  { 61, 0 },
  { 61, 2 },
  { 61, 4 },
  { 65, 3 },
  { 65, 3 },
  { 65, 3 },
  { 65, 3 },
  { 65, 3 },
  { 65, 3 },
  { 65, 3 },
  { 65, 4 },
  { 65, 3 },
  { 65, 3 },
  { 65, 3 },
  { 65, 3 },
  { 65, 3 },
  { 65, 3 },
  { 65, 3 },
  { 65, 3 },
  { 57, 1 },
  { 57, 1 },
  { 57, 2 },
  { 57, 2 },
  { 57, 2 },
  { 57, 2 },
  { 57, 1 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  int yyruleno                 /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  mydbParseARG_FETCH;
  yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno>=0 
        && yyruleno<sizeof(yyRuleName)/sizeof(yyRuleName[0]) ){
    fprintf(yyTraceFILE, "%sReduce [%s].\n", yyTracePrompt,
      yyRuleName[yyruleno]);
  }
#endif /* NDEBUG */

  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
      case 0:
#line 20 "parse.y"
{
  pParse->SetShowTables();
}
#line 777 "parse.c"
        /* No destructor defined for SHOW */
        /* No destructor defined for TABLES */
        /* No destructor defined for SEMI */
        break;
      case 1:
#line 24 "parse.y"
{
  pParse->SetShowTableColumns(&yymsp[-1].minor.yy0);
}
#line 787 "parse.c"
        /* No destructor defined for SHOW */
        /* No destructor defined for COLUMNS */
        /* No destructor defined for FROM */
        /* No destructor defined for SEMI */
        break;
      case 2:
#line 29 "parse.y"
{
  pParse->SetDropTable(&yymsp[-1].minor.yy0);
}
#line 798 "parse.c"
        /* No destructor defined for DROP */
        /* No destructor defined for TABLE */
        /* No destructor defined for SEMI */
        break;
      case 3:
#line 35 "parse.y"
{
  pParse->SetCreateTable(&yymsp[-4].minor.yy0, yymsp[-2].minor.yy125);
}
#line 808 "parse.c"
        /* No destructor defined for CREATE */
        /* No destructor defined for TABLE */
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        /* No destructor defined for SEMI */
        break;
      case 4:
#line 44 "parse.y"
{ yygotominor.yy68 = new ColumnItem(&yymsp[-1].minor.yy0, VALUE_TYPE::VAL_INT); }
#line 818 "parse.c"
        /* No destructor defined for INT_TYPE */
        break;
      case 5:
#line 45 "parse.y"
{ yygotominor.yy68 = new ColumnItem(&yymsp[-1].minor.yy0, VALUE_TYPE::VAL_DOUBLE); }
#line 824 "parse.c"
        /* No destructor defined for DOUBLE_TYPE */
        break;
      case 6:
#line 46 "parse.y"
{ yygotominor.yy68 = new ColumnItem(&yymsp[-1].minor.yy0, VALUE_TYPE::VAL_STRING); }
#line 830 "parse.c"
        /* No destructor defined for STRING_TYPE */
        break;
      case 7:
#line 47 "parse.y"
{
  yygotominor.yy125 = sqlAppendColumn(nullptr, yymsp[0].minor.yy68);
}
#line 838 "parse.c"
        break;
      case 8:
#line 50 "parse.y"
{
  yygotominor.yy125 = sqlAppendColumn(yymsp[-2].minor.yy125, yymsp[0].minor.yy68);
}
#line 845 "parse.c"
        /* No destructor defined for COMMA */
        break;
      case 9:
#line 56 "parse.y"
{
  pParse->SetDelete(&yymsp[-2].minor.yy0, yymsp[-1].minor.yy1);
}
#line 853 "parse.c"
        /* No destructor defined for DELETE */
        /* No destructor defined for FROM */
        /* No destructor defined for SEMI */
        break;
      case 10:
#line 62 "parse.y"
{
  pParse->SetInsert(&yymsp[-8].minor.yy0, yymsp[-6].minor.yy58, yymsp[-2].minor.yy58);
}
#line 863 "parse.c"
        /* No destructor defined for INSERT */
        /* No destructor defined for INTO */
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        /* No destructor defined for VALUES */
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        /* No destructor defined for SEMI */
        break;
      case 11:
#line 69 "parse.y"
{ 
  yygotominor.yy58 = new ExprList();
  yygotominor.yy58->AddExprItem(new ExprItem(TK_ID, &yymsp[0].minor.yy0));
}
#line 879 "parse.c"
        break;
      case 12:
#line 73 "parse.y"
{
  yygotominor.yy58 = yymsp[-2].minor.yy58->AddExprItem(new ExprItem(TK_ID, &yymsp[0].minor.yy0));
}
#line 886 "parse.c"
        /* No destructor defined for COMMA */
        break;
      case 13:
#line 80 "parse.y"
{
  yygotominor.yy58 = new ExprList();
  yygotominor.yy58->AddExprItem(yymsp[0].minor.yy1);
}
#line 895 "parse.c"
        break;
      case 14:
#line 84 "parse.y"
{
  yygotominor.yy58 = yymsp[-2].minor.yy58->AddExprItem(yymsp[0].minor.yy1);
}
#line 902 "parse.c"
        /* No destructor defined for COMMA */
        break;
      case 15:
#line 90 "parse.y"
{
  pParse->SetQuery(yymsp[-5].minor.yy58, &yymsp[-4].minor.yy78, yymsp[-3].minor.yy1, yymsp[-2].minor.yy3, yymsp[-1].minor.yy126);
}
#line 910 "parse.c"
        /* No destructor defined for SELECT */
        /* No destructor defined for SEMI */
        break;
      case 16:
#line 99 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_STAR, &yymsp[0].minor.yy0); }
#line 917 "parse.c"
        break;
      case 17:
#line 100 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_ID, &yymsp[0].minor.yy0); }
#line 922 "parse.c"
        break;
      case 18:
#line 101 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_FUNC, &yymsp[-3].minor.yy0, yymsp[-1].minor.yy58); }
#line 927 "parse.c"
        /* No destructor defined for LP */
        /* No destructor defined for RP */
        break;
      case 19:
#line 103 "parse.y"
{ 
  yygotominor.yy58 = new ExprList();
  yygotominor.yy58->AddExprItem(yymsp[0].minor.yy1);
}
#line 937 "parse.c"
        break;
      case 20:
#line 107 "parse.y"
{
  yygotominor.yy58 = yymsp[-2].minor.yy58->AddExprItem(yymsp[0].minor.yy1);
}
#line 944 "parse.c"
        /* No destructor defined for COMMA */
        break;
      case 21:
#line 116 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_ID, &yymsp[0].minor.yy0); }
#line 950 "parse.c"
        break;
      case 22:
#line 117 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_STAR, &yymsp[0].minor.yy0); }
#line 955 "parse.c"
        break;
      case 23:
#line 119 "parse.y"
{ 
  yygotominor.yy58 = new ExprList();
  yygotominor.yy58->AddExprItem(yymsp[0].minor.yy1);
}
#line 963 "parse.c"
        break;
      case 24:
#line 123 "parse.y"
{
  yygotominor.yy58 = yymsp[-2].minor.yy58->AddExprItem(yymsp[0].minor.yy1);
}
#line 970 "parse.c"
        /* No destructor defined for COMMA */
        break;
      case 25:
#line 128 "parse.y"
{yygotominor.yy78 = yymsp[0].minor.yy0;}
#line 976 "parse.c"
        /* No destructor defined for FROM */
        break;
      case 26:
#line 133 "parse.y"
{ yygotominor.yy1 = nullptr; }
#line 982 "parse.c"
        break;
      case 27:
#line 134 "parse.y"
{ yygotominor.yy1 = yymsp[0].minor.yy1; }
#line 987 "parse.c"
        /* No destructor defined for WHERE */
        break;
      case 28:
#line 139 "parse.y"
{ yygotominor.yy3 = nullptr; }
#line 993 "parse.c"
        break;
      case 29:
#line 140 "parse.y"
{ yygotominor.yy3 = new GroupOpt(&yymsp[0].minor.yy0); }
#line 998 "parse.c"
        /* No destructor defined for GROUP */
        /* No destructor defined for BY */
        break;
      case 30:
#line 145 "parse.y"
{ yygotominor.yy126 = nullptr; }
#line 1005 "parse.c"
        break;
      case 31:
#line 146 "parse.y"
{ yygotominor.yy126 = new LimitOpt(&yymsp[0].minor.yy0); }
#line 1010 "parse.c"
        /* No destructor defined for LIMIT */
        break;
      case 32:
#line 147 "parse.y"
{ yygotominor.yy126 = new LimitOpt(&yymsp[-2].minor.yy0, &yymsp[0].minor.yy0); }
#line 1016 "parse.c"
        /* No destructor defined for LIMIT */
        /* No destructor defined for COMMA */
        break;
      case 33:
#line 158 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_LT, new ExprItem(TK_ID, &yymsp[-2].minor.yy0), yymsp[0].minor.yy1); }
#line 1023 "parse.c"
        /* No destructor defined for LT */
        break;
      case 34:
#line 159 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_LE, new ExprItem(TK_ID, &yymsp[-2].minor.yy0), yymsp[0].minor.yy1); }
#line 1029 "parse.c"
        /* No destructor defined for LE */
        break;
      case 35:
#line 160 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_GT, new ExprItem(TK_ID, &yymsp[-2].minor.yy0), yymsp[0].minor.yy1); }
#line 1035 "parse.c"
        /* No destructor defined for GT */
        break;
      case 36:
#line 161 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_GE, new ExprItem(TK_ID, &yymsp[-2].minor.yy0), yymsp[0].minor.yy1); }
#line 1041 "parse.c"
        /* No destructor defined for GE */
        break;
      case 37:
#line 162 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_EQ, new ExprItem(TK_ID, &yymsp[-2].minor.yy0), yymsp[0].minor.yy1); }
#line 1047 "parse.c"
        /* No destructor defined for EQ */
        break;
      case 38:
#line 163 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_NE, new ExprItem(TK_ID, &yymsp[-2].minor.yy0), yymsp[0].minor.yy1); }
#line 1053 "parse.c"
        /* No destructor defined for NE */
        break;
      case 39:
#line 164 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_LIKE, new ExprItem(TK_ID, &yymsp[-2].minor.yy0), yymsp[0].minor.yy1); }
#line 1059 "parse.c"
        /* No destructor defined for LIKE */
        break;
      case 40:
#line 165 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_ISNOTNULL, new ExprItem(TK_ID, &yymsp[-3].minor.yy0), nullptr); }
#line 1065 "parse.c"
        /* No destructor defined for IS */
        /* No destructor defined for NOT */
        /* No destructor defined for NULL */
        break;
      case 41:
#line 166 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_ISNULL, new ExprItem(TK_ID, &yymsp[-2].minor.yy0), nullptr); }
#line 1073 "parse.c"
        /* No destructor defined for IS */
        /* No destructor defined for NULL */
        break;
      case 42:
#line 168 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_GT, new ExprItem(TK_ID, &yymsp[0].minor.yy0), yymsp[-2].minor.yy1); }
#line 1080 "parse.c"
        /* No destructor defined for LT */
        break;
      case 43:
#line 169 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_GE, new ExprItem(TK_ID, &yymsp[0].minor.yy0), yymsp[-2].minor.yy1); }
#line 1086 "parse.c"
        /* No destructor defined for LE */
        break;
      case 44:
#line 170 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_LT, new ExprItem(TK_ID, &yymsp[0].minor.yy0), yymsp[-2].minor.yy1); }
#line 1092 "parse.c"
        /* No destructor defined for GT */
        break;
      case 45:
#line 171 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_LE, new ExprItem(TK_ID, &yymsp[0].minor.yy0), yymsp[-2].minor.yy1); }
#line 1098 "parse.c"
        /* No destructor defined for GE */
        break;
      case 46:
#line 172 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_EQ, new ExprItem(TK_ID, &yymsp[0].minor.yy0), yymsp[-2].minor.yy1); }
#line 1104 "parse.c"
        /* No destructor defined for EQ */
        break;
      case 47:
#line 173 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_NE, new ExprItem(TK_ID, &yymsp[0].minor.yy0), yymsp[-2].minor.yy1); }
#line 1110 "parse.c"
        /* No destructor defined for NE */
        break;
      case 48:
#line 175 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_AND, yymsp[-2].minor.yy1, yymsp[0].minor.yy1); }
#line 1116 "parse.c"
        /* No destructor defined for AND */
        break;
      case 49:
#line 180 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_INTEGER, &yymsp[0].minor.yy0); }
#line 1122 "parse.c"
        break;
      case 50:
#line 181 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_DOUBLE, &yymsp[0].minor.yy0); }
#line 1127 "parse.c"
        break;
      case 51:
#line 182 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_INTEGER, &yymsp[0].minor.yy0); }
#line 1132 "parse.c"
        /* No destructor defined for PLUS */
        break;
      case 52:
#line 183 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_DOUBLE, &yymsp[0].minor.yy0); }
#line 1138 "parse.c"
        /* No destructor defined for PLUS */
        break;
      case 53:
#line 184 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_UINTEGER, &yymsp[0].minor.yy0); }
#line 1144 "parse.c"
        /* No destructor defined for MINUS */
        break;
      case 54:
#line 185 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_UDOUBLE, &yymsp[0].minor.yy0); }
#line 1150 "parse.c"
        /* No destructor defined for MINUS */
        break;
      case 55:
#line 186 "parse.y"
{ yygotominor.yy1 = new ExprItem(TK_STRING, &yymsp[0].minor.yy0); }
#line 1156 "parse.c"
        break;
  };
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yypParser->yyidx -= yysize;
  yyact = yy_find_reduce_action(yypParser,yygoto);
  if( yyact < YYNSTATE ){
    yy_shift(yypParser,yyact,yygoto,&yygotominor);
  }else if( yyact == YYNSTATE + YYNRULE + 1 ){
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  mydbParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
  mydbParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  YYMINORTYPE yyminor            /* The minor type of the error token */
){
  mydbParseARG_FETCH;
#define TOKEN (yyminor.yy0)
#line 12 "parse.y"

  pParse->SetError(TOKEN.pos_);

#line 1202 "parse.c"
  mydbParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  mydbParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
  mydbParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "mydbParseAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void mydbParse(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  mydbParseTOKENTYPE yyminor       /* The value for the token */
  mydbParseARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  int yyact;            /* The parser action. */
  int yyendofinput;     /* True if we are at the end of input */
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
    if( yymajor==0 ) return;
    yypParser->yyidx = 0;
    yypParser->yyerrcnt = -1;
    yypParser->yystack[0].stateno = 0;
    yypParser->yystack[0].major = 0;
  }
  yyminorunion.yy0 = yyminor;
  yyendofinput = (yymajor==0);
  mydbParseARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput %s\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,yymajor);
    if( yyact<YYNSTATE ){
      yy_shift(yypParser,yyact,yymajor,&yyminorunion);
      yypParser->yyerrcnt--;
      if( yyendofinput && yypParser->yyidx>=0 ){
        yymajor = 0;
      }else{
        yymajor = YYNOCODE;
      }
    }else if( yyact < YYNSTATE + YYNRULE ){
      yy_reduce(yypParser,yyact-YYNSTATE);
    }else if( yyact == YY_ERROR_ACTION ){
      int yymx;
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yymx = yypParser->yystack[yypParser->yyidx].major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yymajor,&yyminorunion);
        yymajor = YYNOCODE;
      }else{
         while(
          yypParser->yyidx >= 0 &&
          yymx != YYERRORSYMBOL &&
          (yyact = yy_find_shift_action(yypParser,YYERRORSYMBOL)) >= YYNSTATE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yyidx < 0 || yymajor==0 ){
          yy_destructor(yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          YYMINORTYPE u2;
          u2.YYERRSYMDT = 0;
          yy_shift(yypParser,yyact,YYERRORSYMBOL,&u2);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }else{
      yy_accept(yypParser);
      yymajor = YYNOCODE;
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
  return;
}
