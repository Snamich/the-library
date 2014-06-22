Scott Chatham
schatham@ucsc.edu

usage:
javac CollidingBalls.java
java CollidingBalls <arg>

This program implements a collision system where the next collision is found using a heap. On each iteration the next collision is found by removing the top object on the heap. The heap uses a minimum item heap property so the collision with the soonest time is always stored on top. After the collision is found we handle it and then update the collision times of all objects that could collide with the two involved in the collision.

In the original version whenever the next collision was found it would search through the entire 2d array of objects to find any additional collisions that happen at the same time. This requires O(n^2) time because we must look at every object. The modified version keeps all collision times stored in a min heap which is guaranteed to sort so that the smallest item is always on top. Whenever we check for the next collision all that is needed is to remove the top element and check to see if the next item has the same collision time. If not, there's only the one collision to handle. If it has the same time then it loops until all similar times have been gotten and then handles these collisions. All of this requires O(nlogn) time because after removing the top we must maintain the properties of the heap which means finding the next smallest element. To do this the last item is put on top and then we percolate it down until we find a position for it that satisfies the heap properties.

Whenever collision times are updated the position of the item being changed is passed along with the new time to the heap's changeKey method. This method alters the collision time of the item given and then looks up to see if it is now smaller than the item above it or checks if it's larger than those below it to find its new position. The heap and the collision time objects are pretty tightly coupled, but this was the only way to be able to access and change an item inside the heap.
