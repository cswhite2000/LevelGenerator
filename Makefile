all: main.o
	gcc -lncurses -g -Wall -Wextra -Werror -o main main.o

main.o: main.c main.h
	gcc -g -Wall -Wextra -Werror -c main.c

clean:
	rm main.o main

