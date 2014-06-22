Scott Chatham
schatham@ucsc.edu

I wrote this on OS X, but it might work on linux as well. The makefile and Angel.h
check what system they're on and conditionally include and link based on that, but
the linux I'm testing on is a virtual machine and there seems to be a weird
interaction going on that causes it to segfault. It works on my mac so if an
issue comes up trying to run it on linux please let me know.
