#ifndef Game_of_Life_cell_h
#define Game_of_Life_cell_h

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct Cell {
    int position, numNeighbors, isAlive, *worldCoordinates;
};

struct Cell* Cell_create( int position, int *, int, int );
void Cell_destroy( struct Cell *);
void Cell_print( struct Cell * );
void Cell_swap( struct Cell *, struct Cell * );

#endif
