UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
CFLAGS=-Wall -g -lGL -lGLU -lglut -lGLEW
endif
ifeq ($(UNAME), Darwin)
CFLAGS=-Wall -g -lGLEW -framework GLUT -framework OpenGL -framework Cocoa
endif

clean:
	rm -f triangle
	rm -rf *.dSYM

all: triangle
