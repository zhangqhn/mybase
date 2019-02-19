%include {
#include "parse.h"
#include "dbval.h"
#include "sql_part.h"
#include "sql_parser.h"
}

%token_prefix  TK_
%token_type    {Token}
%default_type  {Token}
%extra_argument {SQLParser *pParse}
%syntax_error {
  pParse->SetError(TOKEN.pos_);
}
%name mydbParse

%nonassoc  FUNC ILLEGAL SPACE COMMENT.

//////////////////// SHOW TABLES ////////////////////////////////////
cmd ::= SHOW TABLES SEMI. {
  pParse->SetShowTables();
}

cmd ::= SHOW COLUMNS FROM ID(X) SEMI. {
  pParse->SetShowTableColumns(&X);
} 

//////////////////// DROP TABLE /////////////////////////////////
cmd ::= DROP TABLE ID(X) SEMI. {
  pParse->SetDropTable(&X);
}

/////////////////// CREATE Command //////////////////////////////

cmd ::= CREATE TABLE ID(T) LP col_list(C) RP SEMI. {
  pParse->SetCreateTable(&T, C);
}

%type col_item             { ColumnItem* }
%destructor col_item       { delete ($$); }
%type col_list             { ColumnList* }
%destructor col_list       { delete ($$); }

col_item(A) ::= ID(X) INT_TYPE.    { A = new ColumnItem(&X, VALUE_TYPE::VAL_INT); }
col_item(A) ::= ID(X) DOUBLE_TYPE. { A = new ColumnItem(&X, VALUE_TYPE::VAL_DOUBLE); }
col_item(A) ::= ID(X) STRING_TYPE. { A = new ColumnItem(&X, VALUE_TYPE::VAL_STRING); }
col_list(A) ::= col_item(X). {
  A = sqlAppendColumn(nullptr, X);
}
col_list(A) ::= col_list(X) COMMA col_item(Y).  {
  A = sqlAppendColumn(X, Y);
}


//////////////////// DELETE Command /////////////////////////////
cmd ::= DELETE FROM ID(T) where_opt(W) SEMI. {
  pParse->SetDelete(&T, W);
}

//////////////////// INSERT Command ////////////////////////////

cmd ::= INSERT INTO ID(T) LP name_list(N) RP VALUES LP val_list(V) RP SEMI. {
  pParse->SetInsert(&T, N, V);
}

%type name_list           { ExprList* }
%destructor name_list     { delete ($$); }

name_list(A) ::= ID(N).                     { 
  A = new ExprList();
  A->AddExprItem(new ExprItem(TK_ID, &N));
}
name_list(A) ::= name_list(P) COMMA ID(N).  {
  A = P->AddExprItem(new ExprItem(TK_ID, &N));
}

%type val_list          { ExprList* }
%destructor val_list    { delete ($$); }

val_list(A) ::= val_item(V).  {
  A = new ExprList();
  A->AddExprItem(V);
}
val_list(A) ::= val_list(P) COMMA val_item(V).     {
  A = P->AddExprItem(V);
}

/////////////////// Query //////////////////////////////////////
cmd ::= SELECT target_list(S) from(T) where_opt(W) groupby_opt(G) limit_opt(L) SEMI. 
{
  pParse->SetQuery(S, &T, W, G, L);
}

%type target_list               { ExprList* }
%destructor target_list         { delete ($$); }
%type target_item               { ExprItem* }
%destructor target_item         { delete ($$); }

target_item(A) ::= STAR(S).     { A = new ExprItem(TK_STAR, &S); }
target_item(A) ::= ID(X).       { A = new ExprItem(TK_ID, &X); }
target_item(A) ::= ID(X) LP arg_list(L) RP. { A = new ExprItem(TK_FUNC, &X, L); }

target_list(A) ::= target_item(X). { 
  A = new ExprList();
  A->AddExprItem(X);
}
target_list(A) ::= target_list(P) COMMA target_item(X). {
  A = P->AddExprItem(X);
}

%type arg_item                 { ExprItem* }
%destructor arg_item           { delete ($$); }
%type arg_list                 { ExprList* }
%destructor arg_list           { delete ($$); }

arg_item(A) ::= ID(X).         { A = new ExprItem(TK_ID, &X); }
arg_item(A) ::= STAR(X).       { A = new ExprItem(TK_STAR, &X); }

arg_list(A) ::= arg_item(I).   { 
  A = new ExprList();
  A->AddExprItem(I);
}
arg_list(A) ::= arg_list(P) COMMA arg_item(I). {
  A = P->AddExprItem(I);
}

%type from                     {Token}
from(A) ::= FROM ID(X).        {A = X;}

%type where_opt                  { ExprItem* }
%destructor where_opt            { delete ($$); }

where_opt(A) ::= .                      { A = nullptr; }
where_opt(A) ::= WHERE condi_expr(X).   { A = X; }

%type groupby_opt                   { GroupOpt* }
%destructor groupby_opt             { delete ($$); }

groupby_opt(A) ::= .                { A = nullptr; }
groupby_opt(A) ::= GROUP BY ID(X).  { A = new GroupOpt(&X); }

%type limit_opt                     { LimitOpt* }
%destructor limit_opt               { delete ($$); }

limit_opt(A) ::= . { A = nullptr; }
limit_opt(A) ::= LIMIT INTEGER(Y).                   { A = new LimitOpt(&Y); }
limit_opt(A) ::= LIMIT INTEGER(X) COMMA INTEGER(Y).  { A = new LimitOpt(&X, &Y); }

%left AND.
%left EQ NE.
%left GT GE LT LE.

/////////////////////////////////// Condition Expression ////////////

%type condi_expr { ExprItem* }
%destructor condi_expr { delete ($$); }

condi_expr(A) ::= ID(X) LT val_item(Y).   { A = new ExprItem(TK_LT, new ExprItem(TK_ID, &X), Y); }
condi_expr(A) ::= ID(X) LE val_item(Y).   { A = new ExprItem(TK_LE, new ExprItem(TK_ID, &X), Y); }
condi_expr(A) ::= ID(X) GT val_item(Y).   { A = new ExprItem(TK_GT, new ExprItem(TK_ID, &X), Y); }
condi_expr(A) ::= ID(X) GE val_item(Y).   { A = new ExprItem(TK_GE, new ExprItem(TK_ID, &X), Y); }
condi_expr(A) ::= ID(X) EQ val_item(Y).   { A = new ExprItem(TK_EQ, new ExprItem(TK_ID, &X), Y); }
condi_expr(A) ::= ID(X) NE val_item(Y).   { A = new ExprItem(TK_NE, new ExprItem(TK_ID, &X), Y); }
condi_expr(A) ::= ID(X) LIKE val_item(Y). { A = new ExprItem(TK_LIKE, new ExprItem(TK_ID, &X), Y); }
condi_expr(A) ::= ID(X) IS NOT NULL. [ISNOTNULL] { A = new ExprItem(TK_ISNOTNULL, new ExprItem(TK_ID, &X), nullptr); }
condi_expr(A) ::= ID(X) IS NULL. [ISNULL] { A = new ExprItem(TK_ISNULL, new ExprItem(TK_ID, &X), nullptr); }

condi_expr(A) ::= val_item(Y) LT ID(X).   { A = new ExprItem(TK_GT, new ExprItem(TK_ID, &X), Y); }
condi_expr(A) ::= val_item(Y) LE ID(X).   { A = new ExprItem(TK_GE, new ExprItem(TK_ID, &X), Y); }
condi_expr(A) ::= val_item(Y) GT ID(X).   { A = new ExprItem(TK_LT, new ExprItem(TK_ID, &X), Y); }
condi_expr(A) ::= val_item(Y) GE ID(X).   { A = new ExprItem(TK_LE, new ExprItem(TK_ID, &X), Y); }
condi_expr(A) ::= val_item(Y) EQ ID(X).   { A = new ExprItem(TK_EQ, new ExprItem(TK_ID, &X), Y); }
condi_expr(A) ::= val_item(Y) NE ID(X).   { A = new ExprItem(TK_NE, new ExprItem(TK_ID, &X), Y); }

condi_expr(A) ::= condi_expr(X) AND condi_expr(Y).  { A = new ExprItem(TK_AND, X, Y); }

%type val_item       { ExprItem* }
%destructor val_item { delete ($$); }

val_item(A) ::= INTEGER(X).                     { A = new ExprItem(TK_INTEGER, &X); }
val_item(A) ::= DOUBLE(X).                      { A = new ExprItem(TK_DOUBLE, &X); }
val_item(A) ::= PLUS INTEGER(X).                { A = new ExprItem(TK_INTEGER, &X); }
val_item(A) ::= PLUS DOUBLE(X).                 { A = new ExprItem(TK_DOUBLE, &X); }
val_item(A) ::= MINUS INTEGER(X). [UINTEGER]    { A = new ExprItem(TK_UINTEGER, &X); }
val_item(A) ::= MINUS DOUBLE(X).  [UDOUBLE]     { A = new ExprItem(TK_UDOUBLE, &X); }
val_item(A) ::= STRING(X).                      { A = new ExprItem(TK_STRING, &X); }
