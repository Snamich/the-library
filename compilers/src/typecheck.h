#ifndef TYPECHECK_H__
#define TYPECHECK_H__

#include <string>

#include "astree.h"
#include "symtable.h"

void typecheck();
string typecheck_rec(astree *node, SymbolTable *scope);

string typecheckBasetype(astree *node);
string typecheckType(astree *node);
string typecheckFunction(astree *node);
string typecheckStructdef(string ident, string field);
string typecheckReturn(astree *node, SymbolTable *scope);
string typecheckWhile(astree *node, SymbolTable *scope);
string typecheckIf(astree *node, SymbolTable *scope);
string typecheckUnop(astree *node, SymbolTable *scope);
string typecheckBinop(astree *node, SymbolTable *scope);
string typecheckConstant(astree *node);
string typecheckCall(astree *node, SymbolTable *scope);
string typecheckVardecl(astree *node, SymbolTable *scope);
string typecheckVariable(astree *node, SymbolTable *scope);
string typecheckAllocator(astree *node, SymbolTable *scope);

#endif
