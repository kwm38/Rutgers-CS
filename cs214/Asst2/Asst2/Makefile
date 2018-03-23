all: invertedIndex

invertedIndex: invertedIndex.c

	gcc -g -Wall -Werror -fsanitize=address invertedIndex.c -pthread -o invertedIndex

clean:
	rm -rf invertedIndex
