#ifndef libnetfiles_h
#define libnetfiles_h

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <malloc.h>
#include <pthread.h>

#define unrestricted 0
#define exclusive 1
#define transaction 2
#define INVALID_FILE_MODE -300

int netserverinit(char * hostname, int filemode);

ssize_t netwrite(int fildes, const void *buf, size_t nbyte);

int netopen(const char *pathname, int flags);

char *  connectServer(char * message);
#endif
