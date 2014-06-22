UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
CPPFLAGS=-Wall -g -lGLEW -lglui -lGL -lGLU -lglut
endif
ifeq ($(UNAME), Darwin)
CPPFLAGS=-Wall -g -lGLEW -lglui -framework GLUT -framework OpenGL -framework Cocoa
endif

clean:
	rm -rf fractals
	rm -rf *.o
	rm -rf common/*.o
	rm -rf *.dSYM

all: fractals

fractals: common/InitShader.o
