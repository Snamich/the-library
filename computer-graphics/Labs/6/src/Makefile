UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
CPPFLAGS=-Wall -g -lGL -lGLU -lglut -lGLEW
endif
ifeq ($(UNAME), Darwin)
CPPFLAGS=-Wall -g -lGLEW -framework GLUT -framework OpenGL -framework Cocoa
endif

clean:
	rm -rf lab6
	rm -rf *.o
	rm -rf common/*.o
	rm -rf *.dSYM

all: lab6

lab6: common/InitShader.o
