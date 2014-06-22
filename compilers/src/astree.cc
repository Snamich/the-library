#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "astree.h"
#include "stringset.h"
#include "lyutils.h"

astree* new_astree(int symbol, int filenr, int linenr,
                   int offset, const char* lexinfo) {
  astree* tree = new astree();
  tree->symbol = symbol;
  tree->filenr = filenr;
  tree->linenr = linenr;
  tree->offset = offset;
  tree->lexinfo = intern_stringset(lexinfo);
  DEBUGF('f', "astree %p->{%d:%d.%d: %s: \"%s\"}\n",
         tree, tree->filenr, tree->linenr, tree->offset,
         get_yytname(tree->symbol), tree->lexinfo->c_str());
  return tree;
}

astree* new_astree(int symbol, const char* lexinfo) {
  astree* tree = new astree();
  tree->symbol = symbol;
  tree->filenr = 0;
  tree->linenr = 0;
  tree->offset = 0;
  tree->lexinfo = intern_stringset(lexinfo);
  DEBUGF('f', "astree %p->{%d:%d.%d: %s: \"%s\"}\n",
         tree, tree->filenr, tree->linenr, tree->offset,
         get_yytname(tree->symbol), tree->lexinfo->c_str());
  return tree;
}

astree* adopt1(astree* root, astree* child) {
  root->children.push_back(child);
  DEBUGF('a', "%p (%s) adopting %p (%s)\n",
         root, root->lexinfo->c_str(),
         child, child->lexinfo->c_str());
  return root;
}

astree* adopt2(astree* root, astree* left, astree* right) {
  adopt1(root, left);
  adopt1(root, right);
  return root;
}

astree* adopt1sym(astree* root, astree* child, int symbol) {
  root = adopt1(root, child);
  root->symbol = symbol;
  return root;
}

static void dump_node(FILE* outfile, astree* node) {
  fprintf(outfile, "%s  (%s)", 
          get_yytname(node->symbol), node->lexinfo->c_str());
}

static void dump_astree_rec(FILE* outfile, astree* root, int depth) {
  if (root == NULL) return;
  if (root->symbol != EMPTY) {
    fprintf(outfile, "%*s", depth * 2, "");
    switch (root->symbol) {
    case NONTERM:
    case TOK_ALLOC_ARR:
    case TOK_ALLOC_NEW:
    case TOK_VAR_FIELD:
    case TOK_VAR_ARR:
    case TOK_VAR_IDENT:
    case TOK_CALL:
    case TOK_B_GE:
    case TOK_B_GT:
    case TOK_B_LE:
    case TOK_B_LT:
    case TOK_B_NE:
    case TOK_B_EQ:
    case TOK_B_ASSIGN:
    case TOK_B_PLUS:
    case TOK_B_MINUS:
    case TOK_B_MULT:
    case TOK_B_DIV:
    case TOK_U_POS:
    case TOK_U_NEG:
    case TOK_U_NEGATE:
    case TOK_U_CHR:
    case TOK_U_ORD:
    case TOK_RETURN:
    case TOK_IF:
    case TOK_ELSE:
    case TOK_WHILE:
    case TOK_INITDECL:
      fprintf(outfile, "%s", root->lexinfo->c_str());
      break;
    default:
      dump_node(outfile, root);
      break;
    }
    
    fprintf(outfile, "\n");
  }
  
  for (size_t child = 0; child < root->children.size(); ++child) {
    if (root->symbol == EMPTY) {
      dump_astree_rec(outfile, root->children[child], depth);
    } else {
      dump_astree_rec(outfile, root->children[child], depth + 1);
    }
  }
}

astree* add_type(astree* root, string type)
{
  if (root != NULL) root->type = new string(type);
  return root;
}

astree* add_scope(astree* root, string nr)
{
  if (root != NULL) root->scopenr = new string(nr);
  return root;
}

void dump_astree(FILE* outfile, astree* root) {
  dump_astree_rec(outfile, root, 0);
  fflush(NULL);
}

void yyprint(FILE* outfile, unsigned short toknum,
             astree* yyvaluep) {
  if (is_defined_token(toknum)) {
    dump_node(outfile, yyvaluep);
  }else {
    fprintf(outfile, "%s(%d)\n",
            get_yytname(toknum), toknum);
  }
  fflush(NULL);
}

void free_ast(astree* root) {
  while (not root->children.empty()) {
    astree* child = root->children.back();
    root->children.pop_back();
    free_ast(child);
  }
  DEBUGF('f', "free [%p]-> %d:%d.%d: %s: \"%s\")\n",
         root, root->filenr, root->linenr, root->offset,
         get_yytname(root->symbol), root->lexinfo->c_str());
  delete root;
}

void free_ast2(astree* tree1, astree* tree2) {
  free_ast(tree1);
  free_ast(tree2);
}

RCSC("$Id: astree.cc,v 1.1 2013-10-11 18:52:46-07 - - $")

