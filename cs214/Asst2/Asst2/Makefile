all: invertedIndex

invertedIndex: invertedIndex.c

	gcc -g -Wall -Werror -fsanitize=address invertedIndex.c -o invertedIndex

clean:
	rm -rf invertedIndex
