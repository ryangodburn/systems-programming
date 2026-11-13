# Makefile -------------------------------------------
# Authors: Ryan Godburn and Emily King
# Musical Chairs with Threads
# ---------------------------------------------------
CC = g++
CFLAGS = -Wall -g -pthread
TARGET = musicalChairs

all: $(TARGET)

$(TARGET): main.o kid.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o kid.o

main.o: main.cpp kid.h model.h
	$(CC) $(CFLAGS) -c main.cpp

kid.o: kid.cpp kid.h model.h
	$(CC) $(CFLAGS) -c kid.cpp

clean:
	rm -f *.o $(TARGET)
