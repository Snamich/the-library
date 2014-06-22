class MinHeap {
    private int currentSize;
    CollisionTime[] arr;

    MinHeap( int heapSize ) {
	arr = new CollisionTime[heapSize];
	currentSize = 0;
    }

    public CollisionTime peek() {
	// return the item at the top without removing it
	if( currentSize > 0 ) {
	    return arr[0];
	} else {
	    return null;
	}
    }

    public void insert( CollisionTime obj ) {
	arr[currentSize] = obj;
	percolateUp( currentSize );
        ++currentSize;
    }

    public CollisionTime deleteMin() {
        CollisionTime item = arr[0];
	arr[0] = arr[--currentSize];
	percolateDown( 0 );

	return item;
    }

    public void changeKey( int index, double time ) {
        int parent = (index - 1) / 2;
        arr[index].time = time;
        if ( arr[index].compareTo( arr[parent] ) < 0 ) {
            percolateUp( index );
        } else {
            percolateDown( index );
        }
    }

    public void percolateDown( int index ) {
	int smallerChild;
	CollisionTime top = arr[index];
	while ( index < currentSize / 2 ) {
	    int leftChild = 2 * index + 1;
	    int rightChild = leftChild + 1;

	    if ( rightChild < currentSize && arr[leftChild].compareTo( arr[rightChild] ) > 0 ) {
		smallerChild = rightChild;
	    } else {
		smallerChild = leftChild;
	    }

	    if ( top.compareTo( arr[smallerChild] ) < 0 ) {
		break;
	    }

	    arr[index] = arr[smallerChild];
            arr[index].heapPosition = index;
	    index = smallerChild;
	}

	arr[index] = top;
        arr[index].heapPosition = index;
    }

    public void percolateUp( int index ) {
	int parent = (index - 1) / 2;
	CollisionTime bottom = arr[index];

	while ( index > 0 && bottom.compareTo( arr[parent] ) < 0 ) {
	    arr[index] = arr[parent];
            arr[index].heapPosition = index;
	    index = parent;
	    parent = (parent - 1) / 2;
	}

	arr[index] = bottom;
        arr[index].heapPosition = index;
    }

    public void printHeap() {
        for( int i = 0; i < 4; ++i ) {
            System.out.format( "%s ", arr[i].toString() );
        }
        System.out.println();
    }
}
