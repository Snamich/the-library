Scott Chatham
CMPS101 W13
Program 4

A quick note about my program. There must not be any extraneous whitespace in the files or the input reading will signal an error. One of the sample input provided was like this and would not work until the extra whitespace was cleaned.

I implemented my red-black tree using an internal class representing the nodes along with a header class to keep track of various extra data. The node class keeps references to its parent and children as well as its predecessor and successor. The header class has a link to the root of the tree as well as the sentinel nil value that all null children point to. It also maintains the minimum and maximum key contained within the tree. Augmented with this information we can retrieve various information quicker then we could without it.

The printtree method is the required O(n) time, because it is simply an in-order traversal of the tree. This works by starting at the root of a given tree and printing the left child, followed by the root, and finally the right child. In this way the tree is printed in-order and each node is only visited once.

Insert operates in O(log n), but with a couple of constants in front of it. First we have to find the correct position within the tree which takes O(log n) time. Once we have this position, we insert the node and then must check to make sure that the red-black properties still hold. This can mean possible rotations given certain situations and when rotations occur we have to check to ensure that the predecessor and successor of the nodes we have rotated are still intact. If they are not we must fix them which means looking down the tree for their new predecessor and successor.


The find method is O(log n) time because at each step it divides the amount of nodes it must check in half. At each point in the tree we check to make sure the node isn't the nil sentinel because if it is then the key is not in the tree. We start at the root and compare the key we're looking for with the key of the root. If it's smaller then we check the left subtree. If the key is greater then we check the right subtree. Otherwise the current node is the one we're looking for and we set currentNode to it.

There is no actual create method as this is all handled by creating a new RBTree. It is O(1) because all we do is initialize a header node whose root is the nil sentinel.

The predecessor and successor methods both operate in O(1) time because we keep track of a nodes predecessor and successor as the tree is built. This information is maintained by the insert method which checks the current node's predecessor and successor as the new node works its way down the tree. All we have to do is return the predecessor or successor whenever it's asked for.

Printnode is also O(1) because we just have to look at all of the information contained within the node and print it out. There is no tree traversal required since we already have a link to the node that we must print.
