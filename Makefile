CC=g++
CFLAGS=-std=c++11 -Wall -Wpedantic -g

all: scats.o setting.o contact.o log.o
	$(CC) $(CFLAGS) -o scats scats.o setting.o contact.o log.o

scats.o: scats.cpp
	$(CC) $(CFLAGS) -c scats.cpp

setting.o: setting.cpp
	$(CC) $(CFLAGS) -c setting.cpp

contact.o: contact.cpp
	$(CC) $(CFLAGS) -c contact.cpp

log.o: log.cpp
	$(CC) $(CFLAGS) -c log.cpp

clean:
	rm *.o

written:
	wc *.cpp *.h