CC = gcc
CFLAGS = -Wall -g -c
INCLUDES = -I./
LIBS = -L./
WITHLIB = -lmodbus

SRCS = $(wildcard *.c)
HDRS = $(wildcard *.h)
OBJS = $(patsubst %.c, %.o, $(SRCS))

main:$(OBJS)
	$(CC) $(LIBS) $^ $(WITHLIB) -o $@

%.o:%.c $(HDRS)	
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	-rm -rf main *.o *~

PHONY:clean all