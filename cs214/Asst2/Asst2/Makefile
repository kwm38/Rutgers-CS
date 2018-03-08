all: index

index: indexer.c
	gcc -Wall -Werror -fsanitize=address indexer.c -o index

clean:
	rm -rf index
