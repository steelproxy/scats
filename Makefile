CC=g++
CFLAGS=-std=c++11 -Wall -Wpedantic -g

all: scats.o contact.o log.o
	$(CC) $(CFLAGS) -o scats scats.o contact.o log.o

scats.o: scats.cpp
	$(CC) $(CFLAGS) -c scats.cpp

contact.o: contact.cpp
	$(CC) $(CFLAGS) -c contact.cpp

log.o: log.cpp
	$(CC) $(CFLAGS) -c log.cpp

written:
	wc *.cpp *.h