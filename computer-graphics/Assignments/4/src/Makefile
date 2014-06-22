UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
CPPFLAGS=-Wall -g -lGL -lGLU -lglut -lGLEW -lglui
endif
ifeq ($(UNAME), Darwin)
CPPFLAGS=-Wall -g -lGLEW -lglui -framework GLUT -framework OpenGL -framework Cocoa
endif

all: Prog3

clean:
	rm -rf Prog3
	rm -rf *.o
	rm -rf common/*.o
	rm -rf *.dSYM

Prog3: common/InitShader.o
