CC = gcc
OBJ = main.o conf-parser.o xmalloc.o core-util.o

main: $(OBJ)
	$(CC) -g -o $@ $^

main.o: main.c
	$(CC) -c -g $<

conf-parser.o: conf-parser.c conf-parser.h
	$(CC) -c -g $<

xmalloc.o: xmalloc.c xmalloc.h
	$(CC) -c -g $<

core-util.o: core-util.c core-util.h
	$(CC) -c -g $<
