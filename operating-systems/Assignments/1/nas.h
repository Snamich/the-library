/*
  CREATED: Scott Chatham 4/20/13

  nas.h

  This file declares all of the methods defined in the corresponding
  nas.c file.
*/

void DisplayPrompt();
FILE* HandleRedirection( struct Job* );
void ParseArguments( char**, struct Job* );
char** ReadInput();
void RunJob( struct Job* );
