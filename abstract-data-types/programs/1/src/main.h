#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "cell.c"

const int NUM_ROWS = 80;
const int NUM_COLUMNS = 80;
const int MAX_CELLS = 100;

int ApplyRules( struct Cell **, struct Cell *, int * );
int CellExists( int *, struct Cell **, int * );
int ReadInput( FILE *, int[][NUM_COLUMNS], struct Cell**, int * );
void AddCell( int *, struct Cell **, int *, int, int );
void AddNeighbors( int [][NUM_COLUMNS], struct Cell *, struct Cell **,
		   int *, struct Cell **, int * );
void ClearScreen( );
void DeleteCell( int, struct Cell **, int * );
void Die( const char * );
void PrintWorld( int[][NUM_COLUMNS], struct Cell **, int * );
void PrintWorldCoordinates( int[][NUM_COLUMNS], int * );
void Update( int[][NUM_COLUMNS], struct Cell **, int * );
void WaitFor( unsigned int );
