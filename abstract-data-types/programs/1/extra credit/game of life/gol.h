#ifndef GOL_H
#define GOL_H

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "cell.h"

#define NUM_ROWS 59
#define NUM_COLUMNS 48
#define MAX_CELLS 100

int ApplyRules( struct Cell **, struct Cell *, int * );
int CellExists( int *, struct Cell **, int * );
void AddCell( int *, struct Cell **, int *, int, int );
void AddNeighbors( int [NUM_ROWS][NUM_COLUMNS], struct Cell *, struct Cell **,
		   int *, struct Cell **, int * );
void ClearScreen( );
void DeleteCell( int, struct Cell **, int * );
void Die( const char * );
void PrintWorld( int[NUM_ROWS][NUM_COLUMNS], struct Cell **, int * );
void PrintWorldCoordinates( int[][NUM_COLUMNS], int * );
void Update( int[NUM_ROWS][NUM_COLUMNS], struct Cell **, int * );
void WaitFor( unsigned int );

#endif
