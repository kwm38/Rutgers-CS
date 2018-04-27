all: libnetfiles netfileserver

libnetfiles: libnetfiles.c

	gcc -g -Wall -Werror -fsanitize=address libnetfiles.c -pthread -o libnetfiles

netfileserver: netfileserver.c

	gcc -g -Wall -Werror -fsanitize=address netfileserver.c -pthread -o netfileserver

clean:
	rm -rf libnetfiles
	rm -rf netfileserver
