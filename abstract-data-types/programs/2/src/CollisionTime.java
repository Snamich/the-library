class CollisionTime implements Comparable<CollisionTime> {
    double time;
    int objectA;
    int objectB;
    int heapPosition;

    CollisionTime( int objA, int objB, double t ) {
        objectA = objA;
        objectB = objB;
        time = t;
        heapPosition = -1;
    }

    public int compareTo( CollisionTime ct ) {
        return ( new Double( time ) ).compareTo(new Double( ct.time ) );
    }

    public String toString() {
        return "Time: " + time + " Object A: " + objectA + " Object B: " + objectB + " Position: " + heapPosition + "\n";
    }
}
