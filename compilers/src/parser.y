%{

#include "assert.h"
#include "astree.h"
#include "lyutils.h"

%}

%debug
%defines
%error-verbose
%token-table
%verbose

%token TOK_VOID TOK_BOOL TOK_CHAR TOK_INT TOK_STRING
%token TOK_WHILE TOK_RETURN TOK_STRUCT
%token TOK_FALSE TOK_TRUE TOK_NULL TOK_NEW TOK_ARRAY
%token TOK_IDENT TOK_INTCON TOK_CHARCON TOK_STRINGCON

%token TOK_B_NEGATE TOK_B_PLUS TOK_B_ASSIGN TOK_B_MINUS TOK_B_MULT TOK_B_DIV
%token TOK_B_EQ TOK_B_NE TOK_B_LT TOK_B_LE TOK_B_GT TOK_B_GE
%token TOK_U_POS TOK_U_NEG TOK_U_NEGATE TOK_U_ORD TOK_U_CHR
%token TOK_ALLOC_ARR TOK_ALLOC_NEW
%token TOK_VAR_IDENT TOK_VAR_ARR TOK_VAR_FIELD

%token TOK_BLOCK TOK_CALL TOK_IFELSE TOK_INITDECL
%token TOK_TYPEID
%token TOK_ROOT NONTERM EMPTY

%right TOK_IF "then" TOK_ELSE
%right '='
%left TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%left '-' '+'
%left '*' '/'
%right TOK_POS TOK_NEG '!' TOK_ORD TOK_CHR
%left TOK_CALL
%left '[' '.'

%start program

%%

program : program.opt                   { $$ = $1; }
        ;

program.opt : program.opt structdef     { $$ = adopt1($1, $2); }
        | program.opt function          { $$ = adopt1($1, $2); }
        | program.opt statement         { $$ = adopt1($1, $2); }
        |                               { $$ = yyparse_astree = new_astree(NONTERM, "program"); }
        ;

structdef : TOK_STRUCT TOK_IDENT '{' structdef.opt '}'  { astree *a = new_astree(NONTERM, "structdef"); free_ast2($3, $5); $$ = adopt2(a, $2, $4); }
        ;

structdef.opt : structdef.opt decl ';'  { free_ast($3); $$ = adopt1($1, $2); }
        |                               { $$ = new_astree(EMPTY, "empty"); }
        ;

decl : type TOK_IDENT   { astree *a = new_astree(NONTERM, "decl"); $$ = adopt2(a, $1, $2); }
        ;

type : basetype array.opt       { astree *a = new_astree(NONTERM, "type"); $$ = adopt2(a, $1, $2); }
        ;

array.opt : TOK_ARRAY           { $$ = $1; }
        |                       { $$ = new_astree(EMPTY, "empty"); }
        ;

basetype : TOK_VOID     { astree *a = new_astree(NONTERM, "basetype"); $$ = adopt1(a, $1); }
        | TOK_BOOL      { astree *a = new_astree(NONTERM, "basetype"); $$ = adopt1(a, $1); }
        | TOK_CHAR      { astree *a = new_astree(NONTERM, "basetype"); $$ = adopt1(a, $1); }
        | TOK_INT       { astree *a = new_astree(NONTERM, "basetype"); $$ = adopt1(a, $1); }
        | TOK_STRING    { astree *a = new_astree(NONTERM, "basetype"); $$ = adopt1(a, $1); }
        | TOK_IDENT     { astree *a = new_astree(NONTERM, "basetype"); $$ = adopt1(a, $1); }
        ;

function : type TOK_IDENT '(' parameter ')' block       { astree *a = new_astree(NONTERM, "function"); free_ast2($3, $5); adopt2(a, $1, $2); $$ = adopt2(a, $4, $6); }
        ;

parameter : parameter decl      { $$ = adopt1($1, $2); }
        | parameter ','         { free_ast($2); $$ = $1; }
        |                       { $$ = new_astree(NONTERM, "parameters"); }
        ;

block : '{' statement.opt '}'   { astree *a = new_astree(NONTERM, "block"); free_ast2($1, $3); $$ = adopt1(a, $2); }
        | ';'                   { free_ast($1); $$ = new_astree(NONTERM, "block"); }
        ;

statement.opt : statement.opt statement         { $$ = adopt1($1, $2); }
        |                                       { $$ = new_astree(EMPTY, "empty"); }
        ;

statement : block               { $$ = $1; }
        | vardecl               { $$ = $1; }
        | while                 { $$ = $1; }
        | ifelse                { $$ = $1; }
        | return                { $$ = $1; }
        | expr ';'              { free_ast($2); $$ = $1; }
        ;

vardecl : type TOK_IDENT '=' expr ';'           { astree *a = new_astree(TOK_INITDECL, "vardecl"); free_ast2($3, $5); adopt2(a, $1, $2); $$ = adopt1(a, $4); }

while : TOK_WHILE '(' expr ')' statement        { astree *a = new_astree(TOK_WHILE, "while"); free_ast2($2, $4); $$ = adopt2(a, $3, $5); }
        ;

ifelse : TOK_IF '(' expr ')' statement  %prec "then"            { free_ast2($2, $4); $$ = adopt2($1, $3, $5); }
        | TOK_IF '(' expr ')' statement TOK_ELSE statement      { free_ast2($2, $4); adopt1($1, $3); $$ = adopt2($1, $5, adopt1($6, $7)); }
        ;

return : TOK_RETURN expr.opt ';'        { astree *a = new_astree(TOK_RETURN, "return"); free_ast($3); $$ = adopt1(a, $2); }
        ;

expr.opt : expr         { $$ = $1; }
        |               { $$ = new_astree(EMPTY, "empty"); }
        ;

expr : unop             { $$ = $1; }
        | binop         { $$ = $1; }
        | allocator     { $$ = $1; }
        | call          { $$ = $1; }
        | '(' expr ')'  { free_ast2($1, $3); $$ = $2; }
        | variable      { $$ = $1; }
        | constant      { astree *a = new_astree(NONTERM, "constant"); $$ = adopt1(a, $1); }
        ;

binop : expr '=' expr           { astree *a = new_astree(TOK_B_ASSIGN, "binop"); adopt2(a, $1, $2); $$ = adopt1(a, $3); }
        | expr '+' expr         { astree *a = new_astree(TOK_B_PLUS, "binop"); adopt2(a, $1, $2); $$ = adopt1(a, $3); }
        | expr '-' expr         { astree *a = new_astree(TOK_B_MINUS, "binop"); adopt2(a, $1, $2); $$ = adopt1(a, $3); }
        | expr '*' expr         { astree *a = new_astree(TOK_B_MULT, "binop"); adopt2(a, $1, $2); $$ = adopt1(a, $3); }
        | expr '/' expr         { astree *a = new_astree(TOK_B_DIV, "binop"); adopt2(a, $1, $2); $$ = adopt1(a, $3); }
        | expr TOK_EQ expr      { astree *a = new_astree(TOK_B_EQ, "binop"); adopt2(a, $1, $2); $$ = adopt1(a, $3); }
        | expr TOK_NE expr      { astree *a = new_astree(TOK_B_NE, "binop"); adopt2(a, $1, $2); $$ = adopt1(a, $3); }
        | expr TOK_LT expr      { astree *a = new_astree(TOK_B_LT, "binop"); adopt2(a, $1, $2); $$ = adopt1(a, $3); }
        | expr TOK_LE expr      { astree *a = new_astree(TOK_B_LE, "binop"); adopt2(a, $1, $2); $$ = adopt1(a, $3); }
        | expr TOK_GT expr      { astree *a = new_astree(TOK_B_GT, "binop"); adopt2(a, $1, $2); $$ = adopt1(a, $3); }
        | expr TOK_GE expr      { astree *a = new_astree(TOK_B_GE, "binop"); adopt2(a, $1, $2); $$ = adopt1(a, $3); }
        ;

unop : '+' expr %prec TOK_POS           { astree *a = new_astree(TOK_U_POS, "unop"); $$ = adopt2(a, $1, $2); }
        | '-' expr %prec TOK_NEG        { astree *a = new_astree(TOK_U_NEG, "unop"); $$ = adopt2(a, $1, $2); }
        | '!' expr                      { astree *a = new_astree(TOK_U_NEGATE, "unop"); $$ = adopt2(a, $1, $2); }
        | TOK_CHR expr                  { astree *a = new_astree(TOK_U_CHR, "unop"); $$ = adopt2(a, $1, $2); }
        | TOK_ORD expr                  { astree *a = new_astree(TOK_U_ORD, "unop"); $$ = adopt2(a, $1, $2); }
        ;

allocator : TOK_NEW basetype '(' expr.opt ')'   { astree *a = new_astree(TOK_ALLOC_NEW, "allocator"); free_ast2($3, $5); adopt2(a, $1, $2); $$ = adopt1(a, $4); }
        | TOK_NEW basetype '[' expr ']'         { astree *a = new_astree(TOK_ALLOC_ARR, "allocator"); free_ast2($3, $5); adopt2(a, $1, $2); $$ = adopt1(a, $4); }
        ;

call : TOK_IDENT '(' argument ')'       { astree *a = new_astree(TOK_CALL, "call"); free_ast2($2, $4); $$ = adopt2(a, $1, $3); }
        ;

argument : expr                 { astree *a = new_astree(NONTERM, "arguments"); $$ = adopt1(a, $1); }
	| argument ',' expr     { free_ast($2); $$ = adopt1($1, $3); }
        |                       { $$ = new_astree(EMPTY, "empty"); }
        ;

variable : TOK_IDENT            { astree *a = new_astree(TOK_VAR_IDENT, "variable"); $$ = adopt1(a, $1); }
        | expr '[' expr ']'     { astree *a = new_astree(TOK_VAR_ARR, "variable"); free_ast2($2, $4); $$ = adopt2(a, $1, $3); }
        | expr '.' TOK_IDENT    { astree *a = new_astree(TOK_VAR_FIELD, "variable"); free_ast($2); $$ = adopt2(a, $1, $3); }
        ;

constant : TOK_INTCON           { $$ = $1; }
        | TOK_CHARCON           { $$ = $1; }
        | TOK_STRINGCON         { $$ = $1; }
        | TOK_FALSE             { $$ = $1; }
        | TOK_TRUE              { $$ = $1; }
        | TOK_NULL              { $$ = $1; }
        ;

%%

const char *get_yytname(int symbol) {
   return yytname[YYTRANSLATE(symbol)];
}

bool is_defined_token(int symbol) {
   return YYTRANSLATE(symbol) > YYUNDEFTOK;
}
