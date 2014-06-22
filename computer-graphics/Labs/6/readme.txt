Scott Chatham
schatham@ucsc.edu

Lab 6

I wrote this on OS X, but it should work on linux as well. The headers and makefile
check what system they're on and conditionally include and link based on that. The
executable included was compiled on x86 ubuntu 12.04 with all libraries and headers
installed to /usr/.

I implemented picking using axis aligned bounding boxes. The mouse coordinate of
a click is converted to world coordinates (although this isn't quite right yet)
and then it is checked to see if it intersects with any of the bounding boxes. I
didn't get a chance to add the terrain to the lab, but besides a more sophisticated
way to track the selected model it is fairly straightforward.
