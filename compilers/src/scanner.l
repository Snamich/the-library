%{
#include "auxlib.h"
#include "lyutils.h"

#define YY_USER_ACTION  { scanner_useraction (); }
#define IGNORE(THING)   { }
%}

%option 8bit 
%option debug
%option ecs 
%option nodefault 
%option nounput 
%option noyywrap 
%option perf-report 
%option verbose 
%option warn

LETTER          [A-Za-z_]
DIGIT           [0-9]
TOK_IDENT       ({LETTER}({LETTER}|{DIGIT})*)

TOK_INTCON      {DIGIT}+
TOK_CHARCON     ('([^\\'\n]|\\[\\'"0nt])')
TOK_STRINGCON   (\"([^\\"\n]|\\[\\'"0nt])*\")

NOTIDENT        {DIGIT}+{LETTER}+
NOTSTRING       \"([^"\n]|\\.)*
NOTCHAR         ('\\[^\\'"0nt]')|('.[^'])
NOTVALID        {NOTIDENT}|{NOTSTRING}|{NOTCHAR}

%%

"#".*           { scanner_include(); }
[ \t]+          { IGNORE(white space) }
\n              { scanner_newline(); }

{TOK_INTCON}    { return yylval_token(TOK_INTCON); }
{TOK_CHARCON}   { return yylval_token(TOK_CHARCON); }
{TOK_STRINGCON} { return yylval_token(TOK_STRINGCON); }
"ord"           { return yylval_token(TOK_ORD); };
"chr"           { return yylval_token(TOK_CHR); };
"void"          { return yylval_token(TOK_VOID); }
"bool"          { return yylval_token(TOK_BOOL); }
"char"          { return yylval_token(TOK_CHAR); }
"int"           { return yylval_token(TOK_INT); }
"string"        { return yylval_token(TOK_STRING); }
"if"            { return yylval_token(TOK_IF); }
"else"          { return yylval_token(TOK_ELSE); }
"while"         { return yylval_token(TOK_WHILE); }
"return"        { return yylval_token(TOK_RETURN); }
"struct"        { return yylval_token(TOK_STRUCT); }
"false"         { return yylval_token(TOK_FALSE); }
"true"          { return yylval_token(TOK_TRUE); }
"null"          { return yylval_token(TOK_NULL); }
"new"           { return yylval_token(TOK_NEW); }
"[]"            { return yylval_token(TOK_ARRAY); }
"=="            { return yylval_token(TOK_EQ); }
"!="            { return yylval_token(TOK_NE); }
"<"             { return yylval_token(TOK_LT); }
"<="            { return yylval_token(TOK_LE); }
">"             { return yylval_token(TOK_GT); }
">="            { return yylval_token(TOK_GE); }
{TOK_IDENT}     { return yylval_token(TOK_IDENT); }
"="             { return yylval_token('='); }
"+"             { return yylval_token('+'); }
"-"             { return yylval_token('-'); }
"*"             { return yylval_token('*'); }
"/"             { return yylval_token('/'); }
"^"             { return yylval_token('^'); }
"("             { return yylval_token('('); }
")"             { return yylval_token(')'); }
"["             { return yylval_token('['); }
"]"             { return yylval_token(']'); }
"{"             { return yylval_token('{'); }
"}"             { return yylval_token('}'); }
";"             { return yylval_token(';'); }
","             { return yylval_token(','); }
"."             { return yylval_token('.'); }
"!"             { return yylval_token('!'); }
"%"             { return yylval_token('%'); }

{NOTVALID}      { scanner_badtoken(yytext); }

.               { scanner_badchar(*yytext); }

%%
