#include "main.h"

int main( int argc, char *argv[] ) {
    int world[NUM_ROWS][NUM_COLUMNS];
    struct Cell *liveCells[MAX_CELLS];
    int marker = 0, generations, i;

    // process command line arguments
    if ( argc > 3 || argc < 2 ) Die( "USAGE: gameoflife <inputfile> [# generations]" );

    FILE *inputFile = fopen( argv[1], "r" );
    if ( inputFile == NULL ) Die( "Could not open file." );

    generations = ReadInput( inputFile, world, liveCells, &marker );

    if ( argc > 2 ) generations = atoi( argv[2] );

    fclose( inputFile );
    inputFile = NULL;

    ClearScreen();
    printf( "Generation: 0\n" );
    PrintWorld( world, liveCells, &marker );
    WaitFor(1);
    ClearScreen();

    // main loop
    for ( i = 0; i < generations; ++i ) {
	printf( "Generation: %d\n", i + 1 );
	Update( world, liveCells, &marker );
	PrintWorld( world, liveCells, &marker );
	WaitFor(1);
	ClearScreen();
    }

    // clean up
    for ( i = 0; i < marker; ++ i ) {
	Cell_destroy( liveCells[i] );
    }

    return 0;
}

void PrintWorld( int world[][NUM_COLUMNS], struct Cell **liveCells, int *marker ) {
    int i, j;
    for( i = 0; i < 70; ++i ) {
	for( j = 0; j < 70; ++j ) {
	    if ( CellExists( &world[i][j], liveCells, marker ) ) {
		printf( "X" );
	    } else {
		printf( "_" );
	    }
	}
	printf( "\n" );
    }
}

int ReadInput( FILE *input, int world[][NUM_COLUMNS], struct Cell **liveCells, int *marker ) {
    int numGenerations, numArguments, i, row, column;

    fscanf( input, "%d", &numGenerations );
    fscanf( input, "%d", &numArguments );
    for( i = 0; i < numArguments; ++i ) {
	fscanf( input, "%d %d", &row, &column );
	if ( row >= NUM_ROWS || row < 0 || column >= NUM_COLUMNS || column < 0 ) {
	    printf( "Argument %d out of bounds.\n", i );
	    Die( "The world is only 50x50." );
	} else {
	    AddCell( &world[row][column], liveCells, marker, 0, 1 );
	}
    }

    return numGenerations;
}

int CellExists( int *worldCoordinates, struct Cell **liveCells, int *marker ) {
    if ( *worldCoordinates >= 0 && *worldCoordinates < *marker  ) {
	const struct Cell *cell = liveCells[*worldCoordinates];
	if ( cell != NULL ) {
	    if ( &(*worldCoordinates) == cell->worldCoordinates ) {
		return 1;
	    }
	}
    }
    return 0;
}

void DeleteCell( int position, struct Cell **cellsArray, int *marker ) {
    // delete the cell at cellsArray[position]
    // swap cell to be deleted with last cell
    --*marker;
    Cell_swap( cellsArray[position], cellsArray[*marker] );
    Cell_destroy( cellsArray[*marker] );
}

void AddCell( int *worldCoordinates, struct Cell **cellsArray,
	      int *marker, int numNeighbors, int alive ) {
    // create a cell and link it to worldCoordinates
    struct Cell *newCell = Cell_create( *marker, worldCoordinates, numNeighbors, alive );
    *worldCoordinates = newCell->position;
    cellsArray[*marker] = newCell;
    ++*marker;
}

void Update( int world[][NUM_COLUMNS], struct Cell **liveCells, int *marker ) {
    int i, neighborMarker = 0;
    struct Cell *neighborCells[MAX_CELLS*3];

    // find neighbor cells for each live cell
    for ( i = 0; i < *marker; ++i ) {
	AddNeighbors( world, liveCells[i], liveCells, marker, neighborCells, &neighborMarker );
    }

    // apply rules to dead neighbor cells
    int rule;
    for ( i = 0; i < neighborMarker; ++i ) {
	rule = ApplyRules( neighborCells, neighborCells[i], &neighborMarker );
	if ( rule == 2) {
	    neighborCells[i]->isAlive = 1;
	}
    }

    // apply rules to live cells
    for ( i = 0; i < *marker; ) {
	rule = ApplyRules( liveCells, liveCells[i], marker );
	if ( rule == 1 ) {
	    DeleteCell( i, liveCells, marker );
	} else {
	    liveCells[i]->numNeighbors = 0; // go ahead and clear for next update
	    ++i;
	}
    }

    // add live neighbor cells
    for ( i = 0; i < neighborMarker; ++i ) {
	if ( neighborCells[i]->isAlive ) {
	    AddCell( neighborCells[i]->worldCoordinates, liveCells, marker, 0, 1 );
	}
    }

    // release neighbor cells
    for ( i = 0; i < neighborMarker; ++i ) {
	Cell_destroy( neighborCells[i] );
    }
}

void AddNeighbors( int world[][NUM_COLUMNS], struct Cell *currentCell,
		   struct Cell **liveCells, int *marker,
		   struct Cell **neighborCells, int *neighborMarker ) {
    // find all neighbors for current cell and count as we go
    int row, column, j;
    int x = (&(*currentCell->worldCoordinates) - &world[0][0]) / NUM_ROWS;
    int y = (&(*currentCell->worldCoordinates) - &world[0][0]) % NUM_COLUMNS;

    for ( j = 0, row = x - 1, column = y - 1; j < 3; ++j, ++column ) {
	if ( row >= 0 && row < NUM_ROWS && column >= 0 && column < NUM_COLUMNS ) {
	    if ( CellExists( &world[row][column], liveCells, marker ) ) {
		++(currentCell->numNeighbors);
	    } else if ( !CellExists( &world[row][column], neighborCells, neighborMarker ) ) {
		AddCell( &world[row][column], neighborCells, neighborMarker, 1, 0 );
	    } else {
		++(neighborCells[world[row][column]]->numNeighbors);
	    }
	}
    }

    for ( j = 0, row = x + 1, column = y - 1; j < 3; ++j, ++column ) {
	if ( row >= 0 && row < NUM_ROWS && column >= 0 && column < NUM_COLUMNS ) {
	    if ( CellExists( &world[row][column], liveCells, marker ) ) {
		++(currentCell->numNeighbors);
	    } else if ( !CellExists( &world[row][column], neighborCells, neighborMarker ) ) {
		AddCell( &world[row][column], neighborCells, neighborMarker, 1, 0 );
	    } else {
		++(neighborCells[world[row][column]]->numNeighbors);
	    }
	}
    }

    for ( j = 0, row = x, column = y - 1; j < 2; ++j, column+=2 ) {
	if ( row >= 0 && row < NUM_ROWS && column >= 0 && column < NUM_COLUMNS ) {
	    if ( CellExists( &world[row][column], liveCells, marker ) ) {
		++(currentCell->numNeighbors);
	    } else if ( !CellExists( &world[row][column], neighborCells, neighborMarker ) ) {
		AddCell( &world[row][column], neighborCells, neighborMarker, 1, 0 );
	    } else {
		++(neighborCells[world[row][column]]->numNeighbors);
	    }
	}
    }
}

int ApplyRules( struct Cell **cellsArray, struct Cell *cell, int *marker ) {
    int neighbors = cell->numNeighbors;
    if ( cell->isAlive && (neighbors > 3 || neighbors < 2) ) {
	return 1;
    } else if ( !cell->isAlive && neighbors == 3 ) {
	return 2;
    } else {
	return 0;
    }
}

void PrintWorldCoordinates( int world[][NUM_COLUMNS], int *worldCoordinates ) {
    int x = (&(*worldCoordinates) - &world[0][0]) / NUM_ROWS;
    int y = (&(*worldCoordinates) - &world[0][0]) % NUM_COLUMNS;

    printf( "X: %d Y: %d\n", x, y );
}

void WaitFor( unsigned int secs ) {
    int retTime = time( 0 ) + secs;
    while ( time( 0 ) < retTime );
}

void ClearScreen() {
    if (system) {
	system( "clear" );
    } else {
	int i;
	for ( i = 0; i < 100; ++i ) {
	    printf( "\n" );
	}
    }
}

void Die( const char *message ) {
    if ( errno ) {
	perror( message );
    } else {
	printf( "ERROR: %s\n", message );
    }

    exit(1);
}
