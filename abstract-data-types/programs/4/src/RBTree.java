/* Scott Chatham
   CMPS101 W13
   Program 4
*/

public class RBTree {
    private static final int RED = 0;
    private static final int BLACK = 1;

    private RBTreeHeader T;
    private RBTreeNode currentNode;

    public void Insert( int key ) {
        RBTreeNode z = new RBTreeNode( key );
        z.predecessor = T.nil;
        z.successor = T.nil;
        Insert( z );
        if( key > T.max ) {
            T.max = key;
        }
        if( key < T.min ) {
            T.min = key;
        }
    }

    public RBTreeNode Find( int key ) {
        RBTreeNode z = T.root;
        while( z != T.nil ) {
            if( key < z.key ) {
                z = z.left;
            } else if( key > z.key ) {
                z = z.right;
            } else {
                currentNode = z;
                return z;
            }
        }
        return null;
    }

    public int Min() {
        return T.min;
    }

    public int Max() {
        return T.max;
    }

    public void PrintTree() {
        PrintTree( T.root );
    }

    public void PrintCurrentNode() {
        if( currentNode != null ) {
            System.out.println( "Current Node:" );
            PrintNode( currentNode );
        }
    }

    public void CurrentPredecessor() {
        if( currentNode != null ) {
            currentNode = currentNode.predecessor;
        }
    }

    public void CurrentSuccessor() {
        if( currentNode != null ) {
            currentNode = currentNode.successor;
        }
    }

    public RBTree() {
        T = new RBTreeHeader();
        currentNode = T.nil;
    }

    private void Insert( RBTreeNode z ) {
        RBTreeNode y = T.nil;
        RBTreeNode x = T.root;

        while( x != T.nil ) {
            y = x;
            if( z.key < x.key ) {
                if( x.predecessor == T.nil || z.key > x.predecessor.key ) {
                    x.predecessor = z;
                }
                x = x.left;
            } else {
                if( x.successor == T.nil || z.key < x.successor.key ) {
                    x.successor = z;
                }
                x = x.right;
            }
        }

        z.parent = y;
        if( y == T.nil ) {
            T.root = z;
        } else if( z.key < y.key ) {
            y.left = z;
        } else {
            y.right = z;
        }

        z.left = T.nil;
        z.right = T.nil;
        z.color = RED;
        InsertFixup( z );
    }

    private void InsertFixup( RBTreeNode z ) {
        RBTreeNode y;
        while( z.parent != T.nil && z.parent.color == RED ) {
            if( z.parent.parent != T.nil && z.parent == z.parent.parent.left ) {
                y = z.parent.parent.right;
                if( y.color == RED ) {
                    z.parent.color = BLACK;
                    y.color = BLACK;
                    z.parent.parent.color = RED;
                    z = z.parent.parent;
                } else {
                    if( z == z.parent.right ) {
                        z = z.parent;
                        LeftRotate( z );
                    }

                    z.parent.color = BLACK;
                    z.parent.parent.color = RED;
                    RightRotate( z.parent.parent );
                }
            } else {
                y = z.parent.parent.left;
                if( y.color == RED ) {
                    z.parent.color = BLACK;
                    y.color = BLACK;
                    z.parent.parent.color = RED;
                    z = z.parent.parent;
                } else {
                    if( z == z.parent.left ) {
                        z = z.parent;
                        RightRotate( z );
                    }

                    z.parent.color = BLACK;
                    z.parent.parent.color = RED;
                    LeftRotate( z.parent.parent );
                }
            }
        }
        T.root.color = BLACK;
    }

    private void LeftRotate( RBTreeNode x ) {
        RBTreeNode y = x.right;
        x.right = y.left;

        if( y.left != T.nil ) {
            y.left.parent = x;
        }
        y.parent = x.parent;
        if( x.parent == T.nil ) {
            T.root = y;
        } else if( x == x.parent.left ) {
            x.parent.left = y;
        } else {
            x.parent.right = y;
        }

        y.left = x;
        x.parent = y;
        y.successor = FindSuccessor( y );
        y.predecessor = FindPredecessor( y );
        x.successor = FindSuccessor( x );
        x.predecessor = FindPredecessor( x );
    }

    private void RightRotate( RBTreeNode x ) {
        RBTreeNode y = x.left;
        x.left = y.right;
        if( y.right != T.nil ) {
            y.right.parent = x;
        }
        y.parent = x.parent;
        if( x.parent == T.nil ) {
            T.root = y;
        } else if( x == x.parent.right ) {
            x.parent.right = y;
        } else {
            x.parent.left = y;
        }

        y.right = x;
        x.parent = y;
        y.successor = FindSuccessor( y );
        y.predecessor = FindPredecessor( y );
        x.successor = FindSuccessor( x );
        x.predecessor = FindPredecessor( x );
    }

    private void PrintTree( RBTreeNode x ) {
        if( x != null && x != T.nil ) {
            PrintTree( x.left );
            PrintNode( x );
            PrintTree( x.right );
        }
    }
    private RBTreeNode FindSuccessor( RBTreeNode x ) {
        if( x.right != T.nil ) {
            return FindMinimum( x.right );
        } else {
            return T.nil;
        }
    }

    private RBTreeNode FindPredecessor( RBTreeNode x ) {
        if( x.left != T.nil ) {
            return FindMaximum( x.left );
        } else {
            return T.nil;
        }
    }

    private RBTreeNode FindMinimum( RBTreeNode x ) {
        while( x.left != T.nil ) {
            x = x.left;
        }
        return x;
    }

    private RBTreeNode FindMaximum( RBTreeNode x ) {
        while( x.right != T.nil ) {
            x = x.right;
        }
        return x;
    }

    public void PrintNode( RBTreeNode x ) {
        System.out.printf( "Key: %s ", x.key );
        System.out.printf( "Color: %s%n", x.color == RED ? "red" : "black" );
        System.out.printf( "Left: %s%n", x.left == null ? "null" : x.left.key );
        System.out.printf( "Right: %s%n", x.right == null ? "null" : x.right.key );
        System.out.printf( "Predecessor: %s%n", x.predecessor == null ? "null" : x.predecessor.key );
        System.out.printf( "Successor: %s%n%n", x.successor == null ? "null" : x.successor.key );
    }

    private static class RBTreeNode
    {
        RBTreeNode( Integer theKey ) {
            this( theKey, null, null );
        }

        RBTreeNode( Integer theKey, RBTreeNode lt, RBTreeNode rt ) {
            key = theKey;
            left = lt;
            right = rt;
            successor = null;
            predecessor = null;
            color = BLACK;
        }

        Integer key;
        RBTreeNode parent;
        RBTreeNode left;
        RBTreeNode right;
        RBTreeNode successor;
        RBTreeNode predecessor;
        int color;
    }

    private static class RBTreeHeader
    {
        RBTreeHeader() {
            nil = new RBTreeNode( null );
            root = nil;
            min = Integer.MAX_VALUE;
            max = Integer.MIN_VALUE;
        }

        RBTreeNode root;
        RBTreeNode nil;
        int min, max;
    }
}
