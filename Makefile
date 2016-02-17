
CC = gcc
CFLAGS = -c
OBJS = CgiServer.o ServerSocket.o RequestParse.o ServerResponse.o HandlingCGI.o
TARGET = CgiServer

.SUFFIXES : .c .o

all : $(TARGET)

$(OBJS) : CgiServer.h

$(TARGET): $(OBJS)
		$(CC) -o $@ $(OBJS)

clean :
		rm -f $(OBJS) $(TARGET)

