/* Scott Chatham
   CMPS101 W13
   Program 4
*/

import java.util.Scanner;
import java.io.FileReader;
import java.io.IOException;

class Prog4Main {
    public static void main( String [] args ) {
        // create tree
        RBTree tree = new RBTree();

        // start reading input file
        try {
            if( args.length != 0 ) {
                String filename = args[0];
                Scanner in = new Scanner(new FileReader(filename));

                // process commands until we're done
                while( in.hasNextLine() ) {
                    String op = in.next();
                    int key = 1;
                    if( in.hasNextInt() ) {
                        key = in.nextInt();
                    }
                    ProcessCommand( op, key, tree );
                    in.nextLine();
                }
            }
        } catch (IOException e) {
            System.out.println("Invalid input file.");
        }
    }

    public static void ProcessCommand( String op, int key, RBTree tree ) {
        if( op.equals( "i" ) ) {
            tree.Insert( key );
        } else if( op.equals( "pt" ) ) {
            tree.PrintTree();
        } else if( op.equals( "f" ) ) {
            tree.Find( key );
        } else if( op.equals( "pn" ) ) {
            tree.PrintCurrentNode();
        } else if( op.equals( "s" ) ) {
            tree.CurrentSuccessor();
        } else if( op.equals( "p" ) ) {
            tree.CurrentPredecessor();
        } else if( op.equals( "min" ) ){
            System.out.printf( "Min: %d%n", tree.Min() );
        } else if( op.equals( "max" ) ) {
            System.out.printf( "Max: %d%n", tree.Max() );
        } else {
            System.out.printf( "Command %s not recognized.%n", op );
        }
    }
}
