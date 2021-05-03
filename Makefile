CC=g++
CFLAGS=-std=c++11 -Wall -Wpedantic -g -lpthread -lncurses -lboost_system -fpermissive

OBJDIR=obj
SRCDIR=src
BINDIR=bin
TARGET=scats

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
SOURCES	 += $(wildcard $(SRCDIR)/*/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

build: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp $(INCLUDES)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm $(OBJDIR)/*.o $(BINDIR)/$(TARGET)

written:
	wc $(SRCDIR)/*.cpp $(SRCDIR)/*.h