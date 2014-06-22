UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
CPPFLAGS=-Wall -g -lGL -lGLU -lglut -lGLEW
endif
ifeq ($(UNAME), Darwin)
CPPFLAGS=-Wall -g -lGLEW -framework GLUT -framework OpenGL -framework Cocoa
endif

clean:
	rm -rf lab3
	rm -rf *.o
	rm -rf common/*.o
	rm -rf *.dSYM

all: lab3

lab3: common/InitShader.o
