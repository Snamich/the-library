Scott Chatham
schatham@ucsc.edu

Assignment 3

I wrote this on OS X, but it should work on linux as well. The headers and makefile
check what system they're on and conditionally include and link based on that. The
executable included was compiled on x86 ubuntu 12.04 with all libraries and headers
installed to /usr/.

The program uses several globals to make passing of shared data easier, mainly live
variables for GLUI, the initial geometry, and the seed for our random number generator.

There are multiple fractal functions depending on what area of the triangle we're in.
This was done in an attempt to fix holes appearing within the structure. The solution
does not completely work, but it does fix holes at the lower levels and generates
much nicer fractals than the initial version so I decided to include it.

The program can go up to 5 levels of recursion and the GLUI textbox enforces the input
to be within the range of [0,5]. A reset button was also provided to reset the seed
so that multiple versions of the terrain could be generated without having to rerun
the program.
