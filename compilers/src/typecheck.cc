#include "typecheck.h"
#include "symtable.h"
#include "auxlib.h"
#include "lyutils.h"

static int isPrimitiveType(string type);
static int isNull(string type);

void typecheck()
{
  typecheck_rec(yyparse_astree, global_symtable);
}

string typecheck_rec(astree *root, SymbolTable *scope)
{
  string type;
  if (root == NULL) return "";
  if (root->lexinfo->compare("function") == 0) {
    string functionName = root->children[1]->lexinfo->c_str();
    scope = scope->getFunctionScope(functionName);
    typecheckFunction(root);
  } else if (root->lexinfo->compare("block") == 0) {
    scope = scope->getBlockScope(root);
  } else if (root->lexinfo->compare("while") == 0) {
    return typecheckWhile(root, scope);
  } else if (root->lexinfo->compare("if") == 0) {
    return typecheckIf(root, scope);
  } else if (root->lexinfo->compare("basetype") == 0) {
    type = typecheckBasetype(root);
    add_type(root, type_symtable->lookup(type));
    return type;
  } else if (root->lexinfo->compare("type") == 0) {
    type = typecheckType(root);
    add_type(root, type_symtable->lookup(type));
    return type;
  } else if (root->lexinfo->compare("call") == 0) {
    type = typecheckCall(root, scope);
    add_type(root, type_symtable->lookup(type));
    return type;
  } else if (root->lexinfo->compare("variable") == 0) {
    type = typecheckVariable(root, scope);
    add_type(root, type_symtable->lookup(type));
    return type;
  } else if (root->lexinfo->compare("return") == 0) {
    type = typecheckReturn(root, scope);
    add_type(root, type_symtable->lookup(type));
    return type;
  } else if (root->lexinfo->compare("allocator") == 0) {
    type = typecheckAllocator(root, scope);
    add_type(root, type_symtable->lookup(type));
    return type;
  } else if (root->lexinfo->compare("vardecl") == 0) {
    type = typecheckVardecl(root, scope);
    add_type(root, type_symtable->lookup(type));
    return type;
  } else if (root->lexinfo->compare("constant") == 0) {
    type = typecheckConstant(root->children[0]);
    add_type(root, type_symtable->lookup(type));
    return type;
  } else if (root->lexinfo->compare("unop") == 0) {
    type = typecheckUnop(root, scope);
    add_type(root, type_symtable->lookup(type));
    return type;
  } else if (root->lexinfo->compare("binop") == 0) {
    type = typecheckBinop(root, scope);
    add_type(root, type_symtable->lookup(type));
    return type;
  } else {
    
  }
  for (size_t child = 0; child < root->children.size(); ++child) {
    typecheck_rec(root->children[child], scope);
  }
  return "";
}

string typecheckFunction(astree *node)
{
  string type = typecheckType(node->children[0]);

  if (type_symtable->lookup(type) == "" &&
      struct_symtable->lookup(type) == "") {
    errprintf("invalid return type: %s\n", type.c_str());
  }

  return type;
}

string typecheckType(astree *node)
{
  string type = typecheckBasetype(node->children[0]);
  
  if (node->children[1]->symbol == TOK_ARRAY)
    type += "[]";
  return type;
}

string typecheckBasetype(astree *node)
{
  string type = node->children[0]->lexinfo->c_str();
  if (node->children.size() > 1) {
    if (node->children[1]->symbol == TOK_ARRAY)
      type += "[]";
  }
  
  return type;
}

string typecheckReturn(astree *node, SymbolTable *scope)
{
  // get signature of function
  string functionName = node->children[0]->lexinfo->c_str();
  vector<string> args = global_symtable->parseSignature(global_symtable->lookup(functionName));

  // check return type
  string type;
  if (args.size() != 0) {
    type = typecheck_rec(node->children[0], scope);
    if (args[0].compare(type) != 0) {
      errprintf("return type does not match function return\n");
    }
  
    return type;
  }
  return "";
}

string typecheckWhile(astree *node, SymbolTable *scope)
{
  if (typecheck_rec(node->children[0], scope).compare("bool") != 0) {
    errprintf("while expression does not evaluate to bool\n");
  }

  typecheck_rec(node->children[1], scope);

  return "";
}

string typecheckIf(astree *node, SymbolTable *scope)
{
  if (typecheck_rec(node->children[0], scope).compare("bool") != 0) {
    errprintf("if expression does not evaluate to bool\n");
  }

  typecheck_rec(node->children[1], scope);
  if (node->children.size() > 2) typecheck_rec(node->children[2]->children[0], scope);
    
  return "";
}
  
string typecheckUnop(astree *node, SymbolTable *scope)
{
  string type;
  switch (node->symbol) {
  case TOK_U_POS:
    type = typecheck_rec(node->children[1], scope);
    if (type.compare("int") != 0) 
      errprintf("unary + operand type mismatch, expected int and got %s\n", type.c_str());
    return type;
  case TOK_U_NEG:
    type = typecheck_rec(node->children[1], scope);
    if (type.compare("int") != 0) 
      errprintf("unary - operand type mismatch, expected int and got %s\n", type.c_str());
    return type;
  case TOK_U_NEGATE:
    type = typecheck_rec(node->children[1], scope);
    if (type.compare("bool") != 0) 
      errprintf("! operand type mismatch, expected int and got %s\n", type.c_str());
    return type;
  case TOK_U_CHR:
    type = typecheck_rec(node->children[1], scope);
    if (type.compare("int") != 0) 
      errprintf("chr operand type mismatch, expected int and got %s\n", type.c_str());
    return "char";
  case TOK_U_ORD:
    type = typecheck_rec(node->children[1], scope);
    if (type.compare("char") != 0) 
      errprintf("ord operand type mismatch, expected int and got %s\n", type.c_str());
    return "int";
  default:
    errprintf("invalid unop expression\n");
    return "";
  }
}

string typecheckBinop(astree *node, SymbolTable *scope)
{
  string op1, op2;
  switch (node->symbol) {
  case TOK_B_MULT:
    op1 = typecheck_rec(node->children[0], scope);
    if (op1 == "") op1 = "undefined";
    op2 = typecheck_rec(node->children[2], scope);
    if (op2 == "") op2 = "undefined";
    if (op1.compare(op2) != 0 || op1.compare("int") != 0)
      errprintf("* operands must be of type int, not %s and %s\n", op1.c_str(), op2.c_str());
    return "int";
  case TOK_B_DIV:
    op1 = typecheck_rec(node->children[0], scope);
    if (op1 == "") op1 = "undefined";
    op2 = typecheck_rec(node->children[2], scope);
    if (op2 == "") op2 = "undefined";
    if (op1.compare(op2) != 0 || op1.compare("int") != 0)
      errprintf("/ operands must be of type int, not %s and %s\n", op1.c_str(), op2.c_str());
    return "int";
  case TOK_B_PLUS:
    op1 = typecheck_rec(node->children[0], scope);
    if (op1 == "") op1 = "undefined";
    op2 = typecheck_rec(node->children[2], scope);
    if (op2 == "") op2 = "undefined";
    if (op1.compare(op2) != 0 || op1.compare("int") != 0)
      errprintf("+ operands must be of type int, not %s and %s\n", op1.c_str(), op2.c_str());
    return "int";
  case TOK_B_MINUS:
    op1 = typecheck_rec(node->children[0], scope);
    if (op1 == "") op1 = "undefined";
    op2 = typecheck_rec(node->children[2], scope);
    if (op2 == "") op2 = "undefined";
    if (op1.compare(op2) != 0 || op1.compare("int") != 0)
      errprintf("- operands must be of type int, not %s and %s\n", op1.c_str(), op2.c_str());
    return "int";
  case TOK_B_ASSIGN:
    op1 = typecheck_rec(node->children[0], scope);
    if (op1 == "") op1 = "undefined";
    op2 = typecheck_rec(node->children[2], scope);
    if (op2 == "") op2 = "undefined";
    if (isPrimitiveType(op1) && op1.compare(op2) != 0)
      errprintf("= operand types do not match, %s and %s\n", op1.c_str(), op2.c_str());
    else if((!isNull(op1) && !isNull(op2)) && op1.compare(op2) != 0)
      errprintf("= operand types do not match, %s and %s\n", op1.c_str(), op2.c_str());
    return op1;
  case TOK_B_EQ:
    op1 = typecheck_rec(node->children[0], scope);
    if (op1 == "") op1 = "undefined";
    op2 = typecheck_rec(node->children[2], scope);
    if (op2 == "") op2 = "undefined";
    if (isPrimitiveType(op1) && op1.compare(op2) != 0)
      errprintf("== operand types do not match, %s and %s\n", op1.c_str(), op2.c_str());
    else if((!isNull(op1) && !isNull(op2)) && op1.compare(op2) != 0)
      errprintf("== operand types do not match, %s and %s\n", op1.c_str(), op2.c_str());
    return "bool";
  case TOK_B_NE:
    op1 = typecheck_rec(node->children[0], scope);
    if (op1 == "") op1 = "undefined";
    op2 = typecheck_rec(node->children[2], scope);
    if (op2 == "") op2 = "undefined";
    if (isPrimitiveType(op1) && op1.compare(op2) != 0)
      errprintf("!= operand types do not match, %s and %s\n", op1.c_str(), op2.c_str());
    else if((!isNull(op1) && !isNull(op2)) && op1.compare(op2) != 0)
      errprintf("!= operand types do not match, %s and %s\n", op1.c_str(), op2.c_str());
    return "bool";
  case TOK_B_LT:
    op1 = typecheck_rec(node->children[0], scope);
    if (op1 == "") op1 = "undefined";
    op2 = typecheck_rec(node->children[2], scope);
    if (op2 == "") op2 = "undefined";
    if (!isPrimitiveType(op1) || !isPrimitiveType(op2))
      errprintf("< cannot compare types %s and %s\n", op1.c_str(), op2.c_str());
    else if(op1.compare(op2) != 0)
      errprintf("< operands must be the same type: %s %s\n", op1.c_str(), op2.c_str());
    return "bool";
  case TOK_B_LE:
    op1 = typecheck_rec(node->children[0], scope);
    if (op1 == "") op1 = "undefined";
    op2 = typecheck_rec(node->children[2], scope);
    if (op2 == "") op2 = "undefined";
    if (!isPrimitiveType(op1) || !isPrimitiveType(op2))
      errprintf("< cannot compare types %s and %s\n", op1.c_str(), op2.c_str());
    else if(op1.compare(op2) != 0)
      errprintf("< operands must be the same type: %s %s\n", op1.c_str(), op2.c_str());
    return "bool";
  case TOK_B_GT:
    op1 = typecheck_rec(node->children[0], scope);
    if (op1 == "") op1 = "undefined";
    op2 = typecheck_rec(node->children[2], scope);
    if (op2 == "") op2 = "undefined";
    if (!isPrimitiveType(op1) || !isPrimitiveType(op2))
      errprintf("< cannot compare types %s and %s\n", op1.c_str(), op2.c_str());
    else if(op1.compare(op2) != 0)
      errprintf("< operands must be the same type: %s %s\n", op1.c_str(), op2.c_str());
    return "bool";
  case TOK_B_GE:
    op1 = typecheck_rec(node->children[0], scope);
    if (op1 == "") op1 = "undefined";
    op2 = typecheck_rec(node->children[2], scope);
    if (op2 == "") op2 = "undefined";
    if (!isPrimitiveType(op1) || !isPrimitiveType(op2))
      errprintf("< cannot compare types %s and %s\n", op1.c_str(), op2.c_str());
    else if(op1.compare(op2) != 0)
      errprintf("< operands must be the same type: %s %s\n", op1.c_str(), op2.c_str());
    return "bool";
  default:
    errprintf("invalid binop expression\n");
    return "";
  }
}

string typecheckConstant(astree *node)
{
  switch (node->symbol) {
  case TOK_STRINGCON:
    return "string";
  case TOK_INTCON:
    return "int";
  case TOK_CHARCON:
    return "char";
  case TOK_NULL:
    return "null";
  case TOK_FALSE:
    return "bool";
  case TOK_TRUE:
    return "bool";
  default:
    errprintf("invalid constant\n");
    return "";
  }
}

string typecheckCall(astree *node, SymbolTable *scope)
{
  string functionName = node->children[0]->lexinfo->c_str();
  vector<string> args = global_symtable->parseSignature(global_symtable->lookup(functionName));
  
  // check arguments
  string type;
  astree *arguments = node->children[1];
  if (args.size() != 0) {
    for (size_t child = 1; child < arguments->children.size(); ++child) {
      type = typecheck_rec(arguments->children[child-1], scope);
      if (args[child].compare(type) != 0) {
        errprintf("function arguments do not match parameters\n");
      }
    }
    return args[0];
  } else {
    // if args.size() is 0 then it isn't a real function
    errprintf("undeclared function: %s\n", functionName.c_str());
  }

  return "";
}

string typecheckVardecl(astree *node, SymbolTable *scope)
{
  string type = typecheck_rec(node->children[0], scope);
  string rval = typecheck_rec(node->children[2], scope);
  if (type.compare("void") == 0) {
    errprintf("cannot declare variable of type %s\n", type.c_str());
  } else if (type_symtable->lookup(type) == "" &&
             struct_symtable->lookup(type) == "") {
    errprintf("unknown type: %s\n", type.c_str());
  } else if (isPrimitiveType(type) && type.compare(rval) != 0) {
    errprintf("invalid declaration, type mismatch: %s and %s\n", type.c_str(), rval.c_str());
  }
  return type;
}

string typecheckVariable(astree *node, SymbolTable *scope)
{
  string type, index, ident, field, expr_type;
  switch (node->symbol) {
  case TOK_VAR_FIELD:
    expr_type = typecheck_rec(node->children[0], scope);
    ident = node->children[0]->children[0]->lexinfo->c_str();
    field = node->children[1]->lexinfo->c_str();
    type = typecheckStructdef(expr_type, field);
    return type;
  case TOK_VAR_ARR:
    // check string and array
    type = typecheck_rec(node->children[0], scope);
    index = typecheck_rec(node->children[1], scope);
    if (index == "") index = "undefined";
    if (index.compare("int") != 0) {
      errprintf("array index must evaluate to an int, not %s\n", index.c_str());
    }
    if (type.compare("string") == 0) return "char";
    return type.substr(0, type.find_first_of("["));
  case TOK_VAR_IDENT:
    // look up type in symbol table
    ident = node->children[0]->lexinfo->c_str();
    type = scope->lookup(ident);
    if (type.compare("") == 0) errprintf("undeclared variable: %s\n", ident.c_str());
    return type;
  default:
    errprintf("invalid variable expression\n");
    return "";
  }
}

string typecheckAllocator(astree *node, SymbolTable *scope)
{
  string type, index;
  switch (node->symbol) {
  case TOK_ALLOC_ARR:
    type = typecheck_rec(node->children[1], scope);
    type += "[]";
    index = typecheck_rec(node->children[2], scope);
    if (index.compare("int") != 0) {
      errprintf("invalid subscript type in array allocator\n");
    }
    return type;
  case TOK_ALLOC_NEW:
    type = typecheck_rec(node->children[1], scope);
    // basetype errorcheck
    return type;
  default:
    errprintf("invalid allocator expression\n");
    return "";
  }
}

string typecheckStructdef(string ident, string field)
{
  string struct_string = struct_symtable->lookup(ident);
  if (struct_string == "") {
    // ident is not a valid struct
    errprintf("not a valid struct: %s\n", ident.c_str());
    return "";
  }

  vector<string> fields = struct_symtable->parseStruct(struct_string);
  string type;
  for (size_t i = 1; i < fields.size(); i += 2) {
    if (field == fields[i]) {
      // field exists, return its type
      type = fields[i-1];
      //if (node->children[1]->symbol != EMPTY) type += "[]";
      return type;
    }
  }

  errprintf("%s not a valid field for struct %s\n", field.c_str(), ident.c_str());
  return "";
}

int isPrimitiveType(string type)
{
  if (type.compare("int") == 0 || type.compare("bool") == 0 || type.compare("char") == 0) {
    return 1;
  } else {
    return 0;
  }
}

int isNull(string type)
{
  if (type.compare("null") == 0) {
    return 1;
  } else {
    return 0;
  }
}
