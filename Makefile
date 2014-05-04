CC = gcc
OBJ = main.o conf-parser.o xmalloc.o core-util.o

main: $(OBJ)
	$(CC) -o $@ $^

main.o: main.c
	$(CC) -c $<

conf-parser.o: conf-parser.c conf-parser.h
	$(CC) -c $<

xmalloc.o: xmalloc.c xmalloc.h
	$(CC) -c $<

core-util.o: core-util.c core-util.h
	$(CC) -c $<
