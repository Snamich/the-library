#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct Cell {
    int position, numNeighbors, isAlive, *worldCoordinates;
};

struct Cell* Cell_create( int position, int *location, int numNeighbors, int isAlive ) {
    struct Cell *it = malloc( sizeof(struct Cell) );
    assert( it != NULL );

    it->position = position;
    it->numNeighbors = numNeighbors;
    it->isAlive = isAlive;
    it->worldCoordinates = location;

    return it;
}

void Cell_destroy( struct Cell *it ) {
    assert( it != NULL );

    it->position = -1;
    it->worldCoordinates = NULL;
    free( it );
}

void Cell_print( struct Cell *it ) {
    printf( "Position: %d World Coordinates: %p Neighbor Count: %d Alive: %d\n",
	    it->position, it->worldCoordinates, it->numNeighbors, it->isAlive );
}

void Cell_swap( struct Cell *this, struct Cell *that ) {
    assert( this != NULL && that != NULL );
    int *tempCoordinates = this->worldCoordinates;
    int tempStatus = this->isAlive;
    int neighbors = this->numNeighbors;

    this->worldCoordinates = that->worldCoordinates;
    this->numNeighbors = that->numNeighbors;
    this->isAlive = that->isAlive;

    that->worldCoordinates = tempCoordinates;
    that->numNeighbors = neighbors;
    that->isAlive = tempStatus;

    // update world pointers
    *this->worldCoordinates = this->position;
    *that->worldCoordinates = that->position;
}
