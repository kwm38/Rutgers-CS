all: invertedIndex

invertedIndex: invertedIndex.c

	gcc -g -Wall -Werror -fsanitize=address invertedIndex.c -pthread -o invertediNDEX

clean:
	rm -rf invertedIndex
