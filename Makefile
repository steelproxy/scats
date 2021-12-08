CC=g++
CFLAGS=-std=c++11 -Wall -Wpedantic -g -lpthread -lncurses -lfmt -lboost_system -luuid -fpermissive

OBJDIR=obj
SRCDIR=src
BINDIR=bin
TARGET=scats

# $(wildcard *.cpp /xxx/xxx/*.cpp): get all .cpp files from the current directory and dir "/xxx/xxx/"
SRCS := $(shell find . -name "*.cpp")
# $(patsubst %.cpp,%.o,$(SRCS)): substitute all ".cpp" file name strings to ".o" file name strings
OBJS := $(patsubst %.cpp,%.o,$(SRCS))
OBJS := $(patsubst src/%,obj/%,$(SRCS))


COMMAND_SRCS := $(shell find $(SRCDIR)/commands -name "*.cpp")
COMMAND_OBJS := $(patsubst %.cpp,%.o,$(COMMAND_SRCS))
COMMAND_OBJS := $(patsubst src/%,obj/%,$(COMMAND_OBJS))

all: $(BINDIR)/$(TARGET)

cppclean:
	cppclean $(SRCS)

$(BINDIR)/scats: $(OBJDIR)/scats.o $(OBJDIR)/chat/chat_client.o $(OBJDIR)/chat/chat_server.o $(COMMAND_OBJS) $(OBJDIR)/log/log.o $(OBJDIR)/setting/setting.o $(OBJDIR)/chatlog/chatlog.o $(OBJDIR)/commandline/commandline.o $(OBJDIR)/hotkey/hotkey.o $(OBJDIR)/statusline/statusline.o $(OBJDIR)/isprint.o
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(OBJDIR)/scats.o: src/scats.cpp src/scats.h
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/chat/chat_client.o: src/chat/chat_client.cpp src/chat/chat_message.hpp
	mkdir -p $(OBJDIR)/chat
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/chat/chat_server.o: src/chat/chat_server.cpp src/chat/chat_message.hpp
	mkdir -p $(OBJDIR)/commands
	$(CC) $(CFLAGS) -c $< -o $@

$(COMMAND_OBJS): $(COMMAND_SRCS)
	$(CC) $(CFLAGS) -c $< -o $@
	
$(OBJDIR)/log/log.o: src/log/log.cpp src/log/log.h
	mkdir -p $(OBJDIR)/log
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/setting/setting.o: src/setting/setting.cpp src/setting/setting.h src/setting/ini.h
	mkdir -p $(OBJDIR)/setting
	$(CC) $(CFLAGS) -c $< -o $@

#look again
$(OBJDIR)/chatlog/chatlog.o: src/ui/chatlog/chatlog.cpp src/ui/chatlog/chatlog.h
	mkdir -p $(OBJDIR)/chatlog
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/commandline/commandline.o: src/ui/commandline/commandline.cpp src/ui/commandline/commandline.h
	mkdir -p $(OBJDIR)/commandline
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/hotkey/hotkey.o: src/ui/hotkey/hotkey.cpp src/ui/hotkey/hotkey.h
	mkdir -p $(OBJDIR)/hotkey
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/statusline/statusline.o: src/ui/statusline/statusline.cpp src/ui/statusline/statusline.h
	mkdir -p $(OBJDIR)/statusline
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/isprint.o: src/ui/isprint.cpp src/ui/isprint.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJDIR)/*


