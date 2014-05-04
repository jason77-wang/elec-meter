CC = gcc
OBJ = main.o conf-parser.o

main: $(OBJ)
	$(CC) -o $@ $^

main.o: main.c
	$(CC) -c $<

conf-parser.o: conf-parser.c conf-parser.h
	$(CC) -c $<
