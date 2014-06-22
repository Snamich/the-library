UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
CPPFLAGS=-Wall -g -lGL -lGLU -lglut -lGLEW
endif
ifeq ($(UNAME), Darwin)
CPPFLAGS=-Wall -g -lGLEW -framework GLUT -framework OpenGL -framework Cocoa
endif

clean:
	rm -rf lab5
	rm -rf *.o
	rm -rf common/*.o
	rm -rf *.dSYM

all: lab5

lab5: common/InitShader.o
