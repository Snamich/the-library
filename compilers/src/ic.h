#ifndef IC_H__
#define IC_H__

#include <stdio.h>
#include <string>

#include "symtable.h"

string mangle_global(string ident);
string mangle_local(string ident);
string mangle_control(string key, int count);
string mangle_temp(string type);

void print_ic_structs();
void print_ic_globals();
void print_ic_functions();
void print_oil();

string oil_rec(astree *root, SymbolTable *scope);
string ic_binop(astree *root, SymbolTable *scope);
string ic_unop(astree *root, SymbolTable *scope);
string ic_if(astree *root, SymbolTable *scope);
string ic_while(astree *root, SymbolTable *scope);
string ic_allocator(astree *root, SymbolTable *scope);
string ic_call(astree *root, SymbolTable *scope);
string ic_constant(astree *root);
string ic_variable(astree *root, SymbolTable *scope);
string ic_vardecl(astree *root, SymbolTable *scope);

extern FILE *oil_file;

#endif
