CC=g++
CFLAGS=-std=c++11 -Wall -Wpedantic -g -lpthread -lncurses -lfmt -lboost_system -fpermissive

OBJDIR=obj
SRCDIR=src
BINDIR=bin
TARGET=scats

# $(wildcard *.cpp /xxx/xxx/*.cpp): get all .cpp files from the current directory and dir "/xxx/xxx/"
SRCS := $(shell find . -name "*.cpp")
# $(patsubst %.cpp,%.o,$(SRCS)): substitute all ".cpp" file name strings to ".o" file name strings
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $(shell find $(OBJDIR) -name '*.o')
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $(OBJDIR)/$(notdir $@)
clean:
	rm -rf $(TARGET) *.o
	
.PHONY: all clean
