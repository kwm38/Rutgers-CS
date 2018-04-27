#ifndef netfileserver_h
#define netfileserver_h

#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <malloc.h>
#include <signal.h>

#define PORT 52000
/* ----------------------- STRUCTS ----------------------- */
struct fdLL_struct{
    
    int fd;
    char * name;
    int inUse;
    int flags;
    struct fdLL_struct * next;
    struct fdLL_struct * prev;
    
}typedef fdLL;

struct queue_line{
    
    int socket;
    int fileMode;
    char * path;
    int flags;
    int waitTime;
    int closeDown;
    struct queue_line * next;
    struct queue_line * prev;
    
}typedef queue;
/* ----------------------- STRUCTS ----------------------- */

/* ---------------- UTILITY FUNCTIONS ---------------- */
void removeQueueItem(queue * qItem);
int validRequestGivenMode(char * path, int flag, int fileMode);
void storeFd(int fd, char * buffer);
void closeFd(int fd);
void queueLine(int socket, int flag, int fileMode, char * path);
/* ---------------- UTILITY FUNCTIONS ---------------- */

/* ----------------------- OPEN - READ - WRITE - CLOSE ----------------------- */
void openMessage(char * buffer, int socket);
void readMessage(char * buffer, int socket);
void writeMessage(char * buffer, int socket);
void closeMessage(char * buffer, int socket);
/* ----------------------- OPEN - READ - WRITE - CLOSE ----------------------- */

/* ----------------------- THREAD FUNCTIONS ----------------------- */
void * monitorThread(void * qItem);
void * cleanerThread();
void * worker_Thread(void * new_socket);
/* ----------------------- THREAD FUNCTIONS ----------------------- */
#endif
