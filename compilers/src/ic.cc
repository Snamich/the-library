#include <stdio.h>
#include <string>

#include "astree.h"
#include "ic.h"
#include "lyutils.h"
#include "symtable.h"
#include "typecheck.h"

int blocknr, localnr, controlnr;
FILE *oil_file = NULL;

string mangle_global(string ident)
{
  char buff[100];
  sprintf(buff, "__%s", ident.c_str());
  return buff;
}

string mangle_local(string ident)
{
  char buff[100];
  sprintf(buff, "_%d_%s", blocknr, ident.c_str());
  return buff;
}

string mangle_control(string key, int count)
{
  char buff[100];
  sprintf(buff, "%s_%d", key.c_str(), count);
  return buff;
}

string mangle_temp(string type)
{
  char buff[100];
  char typechar = (type == "int ") ? 'i' : (type == "ubyte ") ? 'b' : 'p';
  sprintf(buff, "%c%d", typechar, localnr++);

  return buff;
}

void print_ic_structs()
{
  bool check = false;
  astree *node;
  string name, type, sig, definitions;
  for (size_t child = 0; child < yyparse_astree->children.size(); ++child) {
    node = yyparse_astree->children[child];
    if (node->lexinfo->compare("structdef") == 0) {
      name = node->children[0]->lexinfo->c_str();
      fprintf(oil_file, "struct %s {\n", name.c_str());
      sig = struct_symtable->lookup(name);
      vector<string> definitions = struct_symtable->parseStruct(sig);
      for (size_t def = 0; def < definitions.size(); def += 2) {
        type = type_symtable->lookup(definitions[def]);
        fprintf(oil_file, "%*s%s%s;\n", 8, "", type.c_str(), definitions[def+1].c_str());
      }
      fprintf(oil_file, "};\n");
      check = true;
    }
  }

  if (check) fprintf(oil_file, "\n");
}

void print_ic_globals()
{
  bool check = false;
  astree *node, *name_node, *basetype;
  string name, type, oil_type, mangled_ident;
  for (size_t child = 0; child < yyparse_astree->children.size(); ++child) {
    node = yyparse_astree->children[child];
    if (node->lexinfo->compare("vardecl") == 0) {
      // found a global identifier
      name_node = node->children[1];
      name = name_node->lexinfo->c_str();
      basetype = node->children[0];

      // get the type
      type = basetype->children[0]->children[0]->lexinfo->c_str();
      if (basetype->children[1]->symbol != EMPTY) type += "[]";

      // get the oil type
      oil_type = type_symtable->lookup(type);

      // mangle the name and print it
      mangled_ident = mangle_global(name);
      fprintf(oil_file, "%s%s;\n", oil_type.c_str(), mangled_ident.c_str());
      check = true;
      global_symtable->addName(name, mangled_ident);
    }
  }

  if (check) fprintf(oil_file, "\n");
}

void print_ic_functions()
{
  astree *node, *arguments;
  string name, sig, type, temp;
  vector<string> parameters;
  SymbolTable *scope;
  for (size_t child = 0; child < yyparse_astree->children.size(); ++child) {
    node = yyparse_astree->children[child];
    if (node->lexinfo->compare("function") == 0) {
      // check if the block node has any children
      // if not it's just a prototype and ignore it
      if (node->children[3]->children.size() > 0) {
        // print the header (return type, name, and parameters)
        name = node->children[1]->lexinfo->c_str();
        global_symtable->addName(name, mangle_global(name));
        sig = global_symtable->lookup(name);
        parameters = global_symtable->parseSignature(sig);
        scope = global_symtable->getFunctionScope(name);
        // print the function header
        fprintf(oil_file, "%s\n%s(", type_symtable->lookup(parameters[0]).c_str(), 
                mangle_global(name).c_str());
        if (parameters.size() > 1) {
          // print the parameters
          arguments = node->children[2];
          for (size_t i = 0; i < arguments->children.size(); ++i) {
            if (i == 0) fprintf(oil_file, "\n");
            if (i > 0) fprintf(oil_file, ",\n");
            type = arguments->children[i]->children[0]->type->c_str();
            name = arguments->children[i]->children[1]->lexinfo->c_str();
            temp = mangle_local(name);
            fprintf(oil_file, "%*s%s%s", 8, "", type.c_str(), temp.c_str());
            scope->addName(name, temp);
          }
        }
        // close the parameters
        fprintf(oil_file, ")\n{\n");

        // print the body of the function
        oil_rec(node->children[3], scope);
        
        blocknr++;

        // close it up
        fprintf(oil_file, "}\n\n");
      }
    }
  }
}

string ic_return(astree *root, SymbolTable *scope)
{
  string temp;
  if (root->children[0]->symbol == EMPTY) {
    temp = "";
  } else {
    temp = oil_rec(root->children[0], scope);
  }

  fprintf(oil_file, "%*sreturn %s;\n", 8, "", temp.c_str()); 
  return temp;
}

string ic_binop(astree *root, SymbolTable *scope)
{
  int symbol;
  string name, type, temp, op1, op2;
  if (root->symbol == TOK_B_ASSIGN) {
    symbol = root->children[0]->symbol;
    if ( root->children[0]->lexinfo->compare("variable") == 0) {
      if (symbol == TOK_VAR_FIELD) {
        astree *node = root->children[0];
        name = node->children[0]->children[0]->lexinfo->c_str();
        temp = typecheck_rec(node->children[0], scope);
        type = node->children[1]->lexinfo->c_str();
        op1 = scope->getName(name) + "->" + type;
        op2 = oil_rec(root->children[2], scope);
        fprintf(oil_file, "%*s%s = %s;\n", 8, "", op1.c_str(), op2.c_str());
        temp = typecheckStructdef(temp, type);
        return temp;
      } else if (symbol == TOK_VAR_ARR) {
        astree *node = root->children[0];
        op1 = scope->getName(node->children[0]->children[0]->lexinfo->c_str());
        string index = oil_rec(node->children[1], scope);
        op2 = oil_rec(root->children[2], scope);
        fprintf(oil_file, "%*s%s[%s] = %s;\n", 8, "", op1.c_str(), index.c_str(), op2.c_str());
      } else {
        name = root->children[0]->children[0]->lexinfo->c_str();
        op1 = scope->getName(name);
        op2 = oil_rec(root->children[2], scope);
        fprintf(oil_file, "%*s%s = %s;\n", 8, "", op1.c_str(), op2.c_str());
        return op1;
      }
    }
  } else {
    // get the type of the expr
    type = type_symtable->lookup(typecheck_rec(root, scope));
    // create the temp
    temp = mangle_temp(type);
    // get the operands
    op1 = oil_rec(root->children[0], scope);
    op2 = oil_rec(root->children[2], scope);
    
    fprintf(oil_file, "%*s%s%s = %s %s %s;\n", 8, "", type.c_str(), 
            temp.c_str(), op1.c_str(), root->children[1]->lexinfo->c_str(), op2.c_str());
  }
  
  // return the temp
  return temp;
}

string ic_unop(astree *root, SymbolTable *scope)
{
  string expr, temp, type, op = oil_rec(root->children[1], scope);
  switch (root->symbol) {
  case TOK_U_POS:
  case TOK_U_NEG:
    type = type_symtable->lookup("int");
    temp = mangle_temp(type);
    fprintf(oil_file, "%*s%s%s = %s%s;\n", 8, "", type.c_str(), temp.c_str(), 
            root->children[0]->lexinfo->c_str(), op.c_str());
    return temp;
  case TOK_U_NEGATE:
    type = type_symtable->lookup("bool");
    temp = mangle_temp(type);
    fprintf(oil_file, "%*s%s%s = !%s;\n", 8, "", type.c_str(), temp.c_str(), op.c_str());
    return temp;
  case TOK_U_CHR:
    type = type_symtable->lookup("char");
    temp = mangle_temp(type);
    fprintf(oil_file, "%*s%s%s = (ubyte)%s;\n", 8, "", type.c_str(), temp.c_str(), 
            op.c_str());
    return temp;
  case TOK_U_ORD:
    type = type_symtable->lookup("int");
    temp = mangle_temp(type);
    fprintf(oil_file, "%*s%s%s = (int)%s;\n", 8, "", type.c_str(), temp.c_str(), 
            op.c_str());
    return temp;
  default:
    break;
  }
  return "unop";
}

string ic_if(astree *root, SymbolTable *scope)
{
  string if_label = mangle_control("fi", controlnr);
  string else_label = mangle_control("else", controlnr++);

  string test = oil_rec(root->children[0], scope);
  fprintf(oil_file, "%*sif (!%s) goto %s;\n", 8, "", test.c_str(), else_label.c_str());
  oil_rec(root->children[1], scope);
  fprintf(oil_file, "%*sgoto %s;\n", 8, "", if_label.c_str());
  fprintf(oil_file, "%s:;\n", else_label.c_str());
  if (root->children.size() > 2) // have an else statement
    oil_rec(root->children[2]->children[0], scope);
  fprintf(oil_file, "%s:;\n", if_label.c_str());
  return "if";
}

string ic_while(astree *root, SymbolTable *scope)
{
  string while_label = mangle_control("while", controlnr);
  string break_label = mangle_control("break", controlnr++);

  fprintf(oil_file, "%s:;\n", while_label.c_str());
  string test = oil_rec(root->children[0], scope);
  fprintf(oil_file, "%*sif (!%s) goto %s;\n", 8, "", test.c_str(), break_label.c_str());
  oil_rec(root->children[1], scope);
  fprintf(oil_file, "%*sgoto %s;\n", 8, "", while_label.c_str());
  fprintf(oil_file, "%s:;\n", break_label.c_str());
  return "while";
}

string ic_allocator(astree *root, SymbolTable *scope)
{
  string type, temp, size_type, expr;
  switch (root->symbol) {
  case TOK_ALLOC_NEW:
    type = root->type->c_str();
    temp = mangle_temp(type);
    size_type = type.substr(0, type.find_last_of("*"));
    fprintf(oil_file, "%*s%s%s = xcalloc(1, sizeof(%s));\n", 8, "", type.c_str(),
            temp.c_str(), size_type.c_str());
    return temp;
  case TOK_ALLOC_ARR:
    type = root->type->c_str();
    temp = mangle_temp(type);
    expr = oil_rec(root->children[2], scope);
    size_type = type.substr(0, type.find_last_of("*"));
    fprintf(oil_file, "%*s%s%s = xcalloc(%s, sizeof(%s));\n", 8, "", type.c_str(),
            temp.c_str(), expr.c_str(), size_type.c_str());
    return temp;
  }
  return "allocator";
}

string ic_call(astree *root, SymbolTable *scope)
{
  // get the function signature
  string type, temp, name = root->children[0]->lexinfo->c_str();
  string mangled_name = mangle_global(name);
  string sig = global_symtable->lookup(name);
  vector<string> params = global_symtable->parseSignature(sig);
  if (params.size() < 2) {
    // no argument function
    if (params[0] == "void") {
      // just call the function
      fprintf(oil_file, "%*s%s();\n", 8, "", mangled_name.c_str());
      return "call";
    } else {
      type = type_symtable->lookup(params[0]);
      temp = mangle_temp(type);
      fprintf(oil_file, "%*s%s%s = %s();\n", 8, "", type.c_str(), temp.c_str(), 
              mangled_name.c_str());
      return temp;
    }
  } else {
    string arguments = "";
    astree *args = root->children[1];
    for (size_t arg = 0; arg < args->children.size(); ++arg) {
      if (arg != 0) arguments += ", ";
      string arg_name = oil_rec(args->children[arg], scope);
      arguments += arg_name;
    }
    if (params[0] == "void") {
      fprintf(oil_file, "%*s%s(%s);\n", 8, "", mangled_name.c_str(), arguments.c_str());
      return "call";
    } else {
      type = type_symtable->lookup(params[0]);
      temp = mangle_temp(type);
      fprintf(oil_file, "%*s%s%s = %s(%s);\n", 8, "", type.c_str(), temp.c_str(), 
              mangled_name.c_str(), arguments.c_str());
      return temp;
    }
  }
  return "call";
}

string ic_constant(astree *root)
{
  astree *node = root->children[0];
  if (node->symbol == TOK_FALSE || node->symbol == TOK_NULL) {
    return "0";
  } else if (node->symbol == TOK_TRUE) {
    return "1";
  } else {
    return node->lexinfo->c_str();
  }
}

string ic_variable(astree *root, SymbolTable *scope)
{
  string expr, index, type, ident, name, field, var_name, type1;
  switch (root->symbol) {
  case TOK_VAR_IDENT:
    var_name = root->children[0]->lexinfo->c_str();
    type = type_symtable->lookup(scope->lookup(var_name));
    ident = scope->getName(var_name);
    name = mangle_temp(type);
    fprintf(oil_file, "%*s%s%s = %s;\n", 8, "", type.c_str(), name.c_str(), ident.c_str());
    return name;
  case TOK_VAR_ARR:
    type1 = typecheck_rec(root, scope);
    type = type_symtable->lookup(type1);
    name = mangle_temp(type);
    expr = oil_rec(root->children[0], scope);
    index = oil_rec(root->children[1], scope);
    fprintf(oil_file, "%*s%s%s = %s[%s];\n", 8, "", type.c_str(), name.c_str(), 
            expr.c_str(), index.c_str());
    return name;
  case TOK_VAR_FIELD:
    type1 = typecheck_rec(root, scope);
    type = type_symtable->lookup(type1);
    name = mangle_temp(type);
    expr = oil_rec(root->children[0], scope);
    field = root->children[1]->lexinfo->c_str();
    fprintf(oil_file, "%*s%s%s = %s->%s;\n", 8, "", type.c_str(), name.c_str(), 
            expr.c_str(), field.c_str());
    return name;
  default:
    break;
  }

  return "variable";
}

string ic_vardecl(astree *root, SymbolTable *scope)
{
  // get the name/value of the expr
  string expr = oil_rec(root->children[2], scope);
  string name = root->children[1]->lexinfo->c_str();
  string ident = scope->checkVardecl(name);
  if (ident == "") {
    // hasn't been declared yet
    ident = mangle_local(name);
    scope->addName(name, ident);
    fprintf(oil_file, "%*s%s%s = %s;\n", 8, "", root->type->c_str(), ident.c_str(), expr.c_str());

    return ident;
  }
  fprintf(oil_file, "%*s%s = %s;\n", 8, "", ident.c_str(), expr.c_str());
  return ident;
}

string oil_rec(astree *root, SymbolTable *scope)
{
  if (root == NULL) return "";
  if (root->lexinfo->compare("call") == 0) {
    return ic_call(root, scope);
  } else if (root->lexinfo->compare("block") == 0) {
    scope = scope->getBlockScope(root);
    blocknr++;
  } else if (root->lexinfo->compare("vardecl") == 0) {
    return ic_vardecl(root, scope);
  } else if (root->lexinfo->compare("binop") == 0) {
    return ic_binop(root, scope);
  } else if (root->lexinfo->compare("unop") == 0) {
    return ic_unop(root, scope);
  } else if (root->lexinfo->compare("if") == 0) {
    return ic_if(root, scope);
  } else if (root->lexinfo->compare("while") == 0) {
    return ic_while(root, scope);
  } else if (root->lexinfo->compare("allocator") == 0) {
    return ic_allocator(root, scope);
  } else if (root->lexinfo->compare("variable") == 0) {
    return ic_variable(root, scope);
  } else if (root->lexinfo->compare("constant") == 0) {
    return ic_constant(root);
  } else if (root->lexinfo->compare("return") == 0) {
    return ic_return(root, scope);
  } else if (root->lexinfo->compare("function") == 0) {
    return "";
  } else if (root->lexinfo->compare("structdef") == 0) {
    // already handled so just return
    return "";
  } else {
    // nothing
  }
  // traverse the astree depth first
  for (size_t child = 0; child < root->children.size(); ++child) {
    oil_rec(root->children[child], scope);
  }
  return "";
}

void print_oil()
{
  blocknr = localnr = controlnr = 0;

  // print preamble
  fprintf(oil_file, "#define __OCLIB_C__\n");
  fprintf(oil_file, "#include \"oclib.oh\"\n");

  // print all structs
  fprintf(oil_file, "\n");
  print_ic_structs();

  // global variables
  print_ic_globals();

  // functions
  print_ic_functions();

  // body of program
  fprintf(oil_file, "void __ocmain ()\n{\n");
  oil_rec(yyparse_astree, global_symtable);
  fprintf(oil_file, "}\n");
}
