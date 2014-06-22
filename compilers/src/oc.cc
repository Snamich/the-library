#include <string>
#include <vector>

#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "astree.h"
#include "auxlib.h"
#include "lyutils.h"
#include "ic.h"
#include "stringset.h"
#include "symtable.h"
#include "typecheck.h"

#define USAGE "Usage: oc [-ly] [-@ flag . . .] [-D string] program.oc\n"

const string CPP = "/usr/bin/cpp";
const size_t LINESIZE = 1024;

string program, command, debug_command = " "; 
string ast_output_file, str_output_file, tok_output_file, sym_output_file, oil_output_file;

void yyin_pipe_open(const char* filename) {
  command = CPP;
  command += debug_command;
  command += filename;
  yyin = popen(command.c_str(), "r");
  if (yyin == NULL) {
    syserrprintf(command.c_str());
    exit(get_exitstatus());
  }
}

string check_extension(char *filepath)
{
  size_t extension_position;
  string program_file = basename(filepath);
  extension_position = program_file.find_last_of(".");

  if (extension_position == std::string::npos ||
      program_file.compare(extension_position, 
                           program_file.length(), ".oc"))
    {
      errprintf("Invalid file, oc only operates on .oc files\n");
      exit(EXIT_FAILURE);
    }

  return program_file.substr(0, extension_position);
}

void setup_output_files(string program_name)
{
  program = program_name;
  str_output_file = program_name + ".str";
  tok_output_file = program_name + ".tok";
  ast_output_file = program_name + ".ast";
  sym_output_file = program_name + ".sym";
  oil_output_file = program_name + ".oil";
}

void set_token_file()
{
  FILE *tok_file = fopen(tok_output_file.c_str(), "w");
  if (tok_file == NULL) syserrprintf("Unable to open tok file\n");
  token_filename(tok_file);
}

void parse_options(int argc, char *argv[]) {
  int option, dstring = 0, file_index = 0;
  yy_flex_debug = yydebug = 0;
  while ((option = getopt(argc, argv, "ly@:D:")) != EOF) {
    switch (option) {
    case 'l': yy_flex_debug = 1; break;
    case 'y': yydebug = 1; break;
    case '@': set_debugflags(optarg); break;
    case 'D': dstring = optind - 1; break;
    default:  errprintf ("%:bad option (%c)\n", optopt); break;
    }
  }
  
  // check for file
  if (optind > (argc - 1)) {
    errprintf(USAGE);
    exit(EXIT_FAILURE);
  } else {
    file_index = optind;
  }

  string program_name = check_extension(argv[file_index]);
  setup_output_files(program_name);

  if (dstring) { debug_command = " -D " + string(argv[dstring]) + " "; }

  yyin_pipe_open(argv[file_index]);
  set_token_file();
}

void yyin_pipe_close()
{
  int pclose_rc = pclose(yyin);
  eprint_status(command.c_str(), pclose_rc);
  if (pclose_rc != 0) set_exitstatus(EXIT_FAILURE);
}

void print_str_file()
{
  FILE *str_file = fopen(str_output_file.c_str(), "w");
  if (str_file == NULL) syserrprintf("Unable to open str file\n");
  dump_stringset(str_file);
  fclose(str_file);
}

void print_ast_file()
{
  FILE *ast_file = fopen(ast_output_file.c_str(), "w");
  if (ast_file == NULL) syserrprintf("Unable to open ast file\n");
  dump_astree(ast_file, yyparse_astree);
  fclose(ast_file);
}

void print_sym_file()
{
  FILE *sym_file = fopen(sym_output_file.c_str(), "w");
  if (sym_file == NULL) syserrprintf("Unable to open sym file\n");
  global_symtable->dump(sym_file, 0);
  fclose(sym_file);
}

void create_symtable_rec(SymbolTable *table, astree *root)
{
  if (root == NULL) return;
  if (root->lexinfo->compare("function") == 0) {
    table = table->addFunction(root);
    // ignore function block
    root->children[3]->lexinfo = new string("function-block");
  } else if (root->lexinfo->compare("block") == 0) {
    table = table->enterBlock(root);
  } else if (root->lexinfo->compare("vardecl") == 0) {
    // enter the symbol in the current symtable
    table->addVardecl(root);
  } else if (root->lexinfo->compare("structdef") == 0) {
    struct_symtable->addStruct(root);
  } else {
    // don't care about this node
  }
  
  // traverse the astree depth first
  for (size_t child = 0; child < root->children.size(); ++child) {
    create_symtable_rec(table, root->children[child]);
  }
}

void create_symtable()
{
  global_symtable = new SymbolTable(NULL);
  type_symtable = new SymbolTable(NULL);
  struct_symtable = new SymbolTable(NULL);

  type_symtable->addSymbol("void", "void ");
  type_symtable->addSymbol("string", "ubyte *");
  type_symtable->addSymbol("string[]", "ubyte **");
  type_symtable->addSymbol("char", "ubyte ");
  type_symtable->addSymbol("char[]", "ubyte *");
  type_symtable->addSymbol("int", "int ");
  type_symtable->addSymbol("int[]", "int *");
  type_symtable->addSymbol("bool", "ubyte ");
  type_symtable->addSymbol("bool[]", "ubyte *");

  create_symtable_rec(global_symtable, yyparse_astree);
}

int main(int argc, char *argv[])
{
  set_execname(argv[0]);

  // check for arguments
  if (argc == 1) {
    errprintf(USAGE);
    exit(EXIT_FAILURE);
  }
  
  // parse arguments
  parse_options(argc, argv);

  int parsecode;
  parsecode = yyparse();
  if (parsecode) {
    errprintf("%:parse failed (%d)\n", parsecode);
  } else {
    DEBUGSTMT('a', dump_astree (stderr, yyparse_astree); );
  }

  yyin_pipe_close();

  // dump the utility files
  print_str_file();
  print_ast_file();

  create_symtable();
  print_sym_file();
  
  typecheck();

  if (get_exitstatus() == 0) {
    oil_file = fopen(oil_output_file.c_str(), "w");
    print_oil();
    fclose(oil_file);

    char buffer[1000];
    sprintf(&buffer[0], "gcc -g -o %s -x c %s.oil oclib.c", 
            program.c_str(), program.c_str());
    string call = buffer;
    system(call.c_str());
  }

  return get_exitstatus();
}
