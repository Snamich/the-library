/*
  CREATED: Scott Chatham 4/20/13

  job.h

  This file contains the declaration of the Job structure
  and accompanying functions.
 */

// defines for readability
#define FILE_NULL 0
#define FILE_IN   1
#define FILE_OUT  2

// our struct Job definition
struct Job {
  char *arguments[10];
  char *command;
  char *filename;
  int redirection;
  int wait;
};

/*
  Job_print
  RETURNS: void
  ARGUMENTS: job - a struct Job

  This function simply prints out the contents of a job for debugging purposes.
 */
void Job_print( struct Job *job )
{
  int i;
  printf( "Command: %s\n", job->command );
  printf( "Arguments: ");
  for( i = 0; job->arguments[i] != NULL; ++i ) {
    printf( "%s ", job->arguments[i] );
  }
  printf( "\n" );
  printf( "Redirection: %s\n", job->redirection == FILE_NULL ? "no" :
          job->redirection == FILE_IN ? "in" : "out" );
  printf( "Filename: %s\n", job->redirection != FILE_NULL ?
          job->filename : "no file" );
  printf( "Waiting: %s\n", job->wait == 1 ? "yes" : "no" );
}

/*
  Job_reset
  RETURNS: void
  ARGUMENTS: job - a struct Job

  This function clears out all the fields of the given job.
*/
void Job_reset( struct Job *job )
{
  int i;
  for( i = 0; job->arguments[i] != NULL; ++i ) {
    job->arguments[i] = NULL;
  }

  job->command = NULL;
  job->filename = NULL;
  job->redirection = FILE_NULL;
  job->wait = 1;
}
