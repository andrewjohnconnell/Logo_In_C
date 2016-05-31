CFLAGS = `sdl2-config --cflags` -O3 -Wall -pedantic -std=c99 -lm -g
 
TARGET = turtles

SOURCES = sdlShorthand.c turtles.c 

LIBS = `sdl2-config --libs`
CC = gcc 


all: $(TARGET)

 $(TARGET): $(SOURCES)
	$(CC) $(SOURCES) -o $(TARGET) $(CFLAGS) $(LIBS)

