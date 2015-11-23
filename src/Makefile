# A basic Makefile for the shellex code in the Systems Programming Book
CC = gcc
CFLAGS = -Wall -g
INCLUDES = -I.
LIBS = -lpthread -ldl
SRCS = upsh.c csapp.c p3extra.c
OBJS = upsh.o csapp.o p3extra.o
EXE = upsh

all:	$(EXE)

$(EXE):	$(OBJS)
	$(CC) -o $(EXE) $(CFLAGS) $(INCLUDES) $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(EXE)
