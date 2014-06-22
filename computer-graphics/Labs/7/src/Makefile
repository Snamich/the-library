UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
CPPFLAGS=-Wall -g -lGL -lGLU -lglut -lGLEW
endif
ifeq ($(UNAME), Darwin)
CPPFLAGS=-Wall -g -lGLEW -framework GLUT -framework OpenGL -framework Cocoa
endif

all: lab7

clean:
	rm -rf lab7
	rm -rf *.o
	rm -rf common/*.o
	rm -rf *.dSYM

lab7: common/InitShader.o
