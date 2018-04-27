#include "netfileserver.h"

fdLL * head = NULL;
queue * qHead = NULL;
int ret1 = -1;
int ret2 = 0;
pthread_mutex_t dataLock;
pthread_mutex_t queueLock;
pthread_mutex_t errnoBlock;

/* ---------------- UTILITY FUNCTIONS ---------------- */

/* inserts a new socket into the queue */
void queueLine(int socket, int flag, int fileMode, char * path){
    
    if(qHead == NULL){
        qHead = (queue *) malloc(sizeof(queue));
        qHead -> socket = socket;
        qHead -> flags = flag;
        qHead -> fileMode = fileMode;
        qHead -> path = (char*) malloc(sizeof(char) * strlen(path) + 1);
        strcpy(qHead -> path, path);
        qHead -> next = qHead -> prev = NULL;
    }else{
        qHead->prev = (queue *) malloc(sizeof(queue));
        qHead->prev -> socket = socket;
        qHead->prev -> flags = flag;
        qHead->prev -> fileMode = fileMode;
        qHead->prev -> path = (char*) malloc(sizeof(char) * strlen(path) + 1);
        strcpy( qHead->prev -> path, path);
        qHead->prev -> prev = NULL;
        qHead->prev -> next = qHead;
        qHead =  qHead->prev;
    }
    
    pthread_t queueMonitorThread;
    if(pthread_create(&queueMonitorThread, NULL, monitorThread, (void *)qHead) !=0) {
        printf("Error creating thread\n");
        exit(0);
    }
    return;
}


/* removes a queued socket from the queue */
void removeQueueItem(queue * qItem){
    /*go through the queue and find the item that matches the arguement*/
    queue * temp = qHead;
    
    
    int count = 1;
    while(qHead != qItem){
        qHead = qHead -> next;
        count ++;
    }
    
    /* here we will update the queue */
    if(qHead -> prev != NULL){
        qHead -> prev -> next = qHead -> next;
    }
    
    if(qHead -> next != NULL){
        qHead -> next -> prev = qHead -> prev;
    }
    
    if(qHead == temp){
        if(qHead->next != NULL){
            qHead = qHead -> next;
            free(qHead -> prev);
            qHead->prev = NULL;
        }else{
            free(qHead);
            qHead = NULL;
        }
    }else {
        free(qHead);
        qHead = temp;
    }
}


/* lets us know if we can open a file given filemode and flags */
int validRequestGivenMode(char * path, int flags, int fileMode){
    
    /* unrestricted mode - so return 1 no matter what */
    if(fileMode == 0){
        return 1;
    }
    fdLL * temp = head;
    
    while(temp != NULL){
        
        /* first we are checking exlcusive mode */
        if(fileMode == 1){
            /* find a stored fd: if its name matches and its in write or read/write mode lets grab it */
            if(strcmp(temp->name,path) == 0){
                /* we have this same file open with write permissions, so we cant have multiple */
                if(flags > 0){
                    /* cant open the file because we already have it open in write mode */
                    /*we can not open the file*/
                    return 0;
                }
            }
        } /* end of exclusive mode check */
        /* now we are checking transaction mode */
        if(fileMode == 2){
            /* find a stored fd: if its name matches and its in write or read/write mode lets grab it */
            if(strcmp(temp->name,path) == 0){
                /* we have this same file open, so we cant have multiple */
                return 0;
            }
        } /* end of transaction mode check */
        temp = temp -> next;
    }
    /* we can proceed with opening the file */
    return 1;
}



/* stores an open fd in a linked list */
void storeFd(int fd, char * buffer){
    
    /* if we have no stored file descriptors store the new one in head */
    if(head == NULL){
        head = (fdLL *) malloc(sizeof(fdLL));
        head -> next = head -> prev = NULL;
        head -> fd = fd;
        head -> flags = buffer[2];
        head -> name = (char*) malloc(sizeof(char) * strlen(buffer) + 1);
        strcpy(head -> name, (buffer+6));
        return;
    }
    /* else add the new fd to the beggining of the LL*/
    /* build a new stored instance of a fd */
    head -> prev = (fdLL *) malloc(sizeof(fdLL));
    head -> prev -> next =  head;
    head -> prev -> prev = NULL;
    head -> prev -> fd = fd;
    head -> prev -> flags = atoi(buffer + 2);
    head -> prev -> name = (char*) malloc(sizeof(char) * strlen(buffer) + 1);
    strcpy(head -> prev -> name, (buffer+6));
    head = head->prev;
    return;
}


/* removes a file descriptor from the linked list */
void closeFd(int fd){
    
    fdLL* temp = head;
    /* once we find our stored file descriptor - remove it */
    while(head != NULL){
        if(head -> fd == fd){
            if(head == temp){
                if(head->next != NULL){
                    fdLL* delete = head;
                    head = head ->next;
                    free(delete);
                    delete = NULL;
                }else{
                    free(head);
                    head = NULL;
                }
                break;
            }else{
                if(head -> next != NULL){
                    
                    head -> next -> prev = head -> prev;
                }
                if (head -> prev != NULL){
                    head->prev->next = head -> next;
                }
                free(head);
                head = NULL;
                head = temp;
                break;
            }
        }
        head = head->next;
    }
    return;
}

/* ---------------- END OF UTILITY FUNCTIONS ---------------- */






/* ----------------------- OPEN - READ - WRITE - CLOSE ----------------------- */
/* this will open a file descriptor or return an error*/
void openMessage(char * buffer, int socket){
    /* -- message to be parsed --
     * o:flags:fileMode:path
     */
    int fileMode = atoi(buffer + 4);  /* extract the file mode */
    int flag = atoi(buffer + 2); /* extract the flag */
    
    /*sending filename,flags,filemode to validRequestGivenMode */
    pthread_mutex_lock(&dataLock); /*lock down the data*/
    int proceed = validRequestGivenMode(buffer + 6, flag, fileMode);
    pthread_mutex_unlock(&dataLock);/*unlock the data*/
    
    char returnMessage[15]; /* build a return message that can handle a large number*/
    if(proceed){
        pthread_mutex_lock(&errnoBlock);/*lock down the errno*/
        int fd = open((buffer + 6), flag); /* extract the file descriptor */
        if(fd > 0){
            /* put the fd in the message to return  */
            snprintf(returnMessage, 15, "%d", fd);
            /* we were able to open the file so store it */
            
            pthread_mutex_lock(&dataLock);   /*lock down the data*/
            storeFd(fd, buffer);
            pthread_mutex_unlock(&dataLock); /*unlock the data*/
            
        }else{
            /* we were un-able to open the file so store the errno in a return message */
            snprintf(returnMessage, 15, "%d", -(errno));
        }
    }else{/* we could not proceed due to permissions and our fileMode so return access error */
        
        /* For extension A we add a socket to the waiting queue for proceed permission
         * without extension A -> snprintf(returnMessage, 15, "%d", -(EACCES));
         */
        
        pthread_mutex_unlock(&errnoBlock);    /*unlock the errno*/
        
        pthread_mutex_lock(&queueLock);/*lock down the data*/
        queueLine(socket, flag, fileMode,buffer + 6);
        pthread_mutex_unlock(&queueLock);/*unlock the data*/
        return;
    }
    
    pthread_mutex_unlock(&errnoBlock); /*unlock the errno*/
    /* send our message to the client containing a fd or errno */
    send(socket, returnMessage , strlen(returnMessage) , 0);
    close(socket);
    return;
}





/* this will read a file descriptor or return an error*/
void readMessage(char * buffer, int socket){
    /* -- message to be parsed --
     * r:size:fd:fileMode:message
     */
    /* setting up our variables needed in order to read/ store read data*/
    ssize_t bytes = 0;
    sscanf(buffer + 2, "%zu", &bytes);
    
    
    int i,count,fdIndex,fileModeIndex,notSet1, notSet2;
    i = count = fdIndex = fileModeIndex = 0;
    notSet1 = notSet2 = 1;
    
    /* find the indecies of the colons */
    while(count != 3){
        if(buffer[i] == ':'){
            count ++;
        }
        if(count == 2 && notSet1){
            fdIndex = i + 1;
            /* we no longer need to update fdIndex as it has been set */
            notSet1 = 0;
        }
        if(count == 3 && notSet2){
            fileModeIndex = i + 1;
            /* we no longer need to update fileModeIndex as it has been set */
            notSet2 = 0;
        }
        i++;
    }
    
    int fd = atoi(buffer + fdIndex); /* get the fd */
    
    pthread_mutex_lock(&errnoBlock); /*lock down the errno*/

    /* malloc space for the message to send back to the client */
    char * returnMessageRead = (char *) malloc(sizeof(char) * (bytes + 1));
    /* if we could not malloc that requested size for our message tell the client we encountered an error */
    if(malloc_usable_size(returnMessageRead) != bytes + 1){
        
        returnMessageRead = (char *) realloc(returnMessageRead, sizeof(char) * 16);
        snprintf(returnMessageRead, 15, "%d", -(errno));
        pthread_mutex_unlock(&errnoBlock);   /*unlock the errno*/
        send(socket, returnMessageRead , strlen(returnMessageRead) , 0);
        close(socket);
        return;
    }
    
    pthread_mutex_unlock(&errnoBlock);/*unlock the errno*/
    /* read from the file and send it to our return message intermediate buffer
     * incase a file has nothing to read we dont want to wait for something
     * to be written to the file, so make it non-blocking
     */
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    /*perform a local read on the fd*/
    
    pthread_mutex_lock(&errnoBlock); /*lock down the errno*/
    pthread_mutex_lock(&dataLock);   /*lock down the data*/
    int readValue = read(fd, returnMessageRead, bytes);
    pthread_mutex_unlock(&dataLock);   /*unlock the data*/
    
    /*build the final message to send to the client */
    char * returnMessage = NULL;
    
    if(readValue > 0){
        /* we were able to read so send back  what we read  and the size we read */
        char readNum [15];
        snprintf(readNum, 15, "%d", readValue);
        returnMessage = (char *) realloc (returnMessage, sizeof(char) * bytes + 16);
        strcat(returnMessage,readNum);
        strcat(returnMessage,":");
        strcat(returnMessage,returnMessageRead);
    }else{
        /* we were unable to read so send back just an error */
        returnMessage = (char *) realloc(returnMessage, sizeof(char) * 16);
        snprintf(returnMessage, 15, "%d", -(errno));
    }
    
    pthread_mutex_unlock(&errnoBlock);/*unclock the errno*/
    /* send our message to the client */
    send(socket, returnMessage , strlen(returnMessage) , 0);
    close(socket);
    return;
}




/* this will write to a file descriptor or return an error*/
void writeMessage(char * buffer, int socket){
    /* -- message to be parsed --
     * w:size:fd:message
     */
    ssize_t bytes = 0;
    sscanf(buffer + 2, "%zu", &bytes);
    
    int i,count,colon1,colon2,notSet1,notSet2;
    i = count = colon1 = colon2 = 0;
    notSet1 = notSet2 = 1;
    /* find the indecies of all the colons that break up the message */
    while(count != 3){
        if(buffer[i] == ':' ){
            count ++;
            if(count == 2 && notSet1){
                colon1 = i;
                /* we no longer need to update colon1 as it has been set */
                notSet1 = 0;
            }else if (count == 3 && notSet2){
                colon2 = i;
                /* we no longer need to update colon2 as it has been set */
                notSet1 = 0;
            }
        }
        i++;
    }
    /*get the file descriptor */
    int fd = atoi(buffer + colon1 + 1);
    
    /* find the stored fd */
    pthread_mutex_lock(&dataLock);   /*lock down the data*/
    fdLL * temp = head;
    while(temp->fd != fd){
        temp = temp->next;
    }
    pthread_mutex_unlock(&dataLock);   /*unlock the data*/
    
    /* set up a buffer to store our message we wish to write */
    char * writeMessage = (char *) malloc(sizeof(char) * bytes + 1);
    /* transfer our message to the buffer */
     strncpy(writeMessage, buffer + colon2 + 1, bytes);
    /*get the flags from our message and make the write non-blocking*/
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    
    pthread_mutex_lock(&errnoBlock); /*lock down the errno*/
    /* write to the file */
    
    pthread_mutex_lock(&dataLock); /*lock down the data*/
    int writeMe = write(fd,writeMessage, strlen(writeMessage));
    pthread_mutex_unlock(&dataLock);/*unlick the data*/
    
    /* build a message that can handle a large number to send to the client */
    char writeNum [15];
    if(writeMe > 0){
        /* we wrote something so record how much we wrote */
        snprintf(writeNum, 15, "%d", writeMe);
        
    }else{
        /* we could not write so lets send back an error */
        if(temp->flags == 0){
            snprintf(writeNum, 15, "%d", -(EROFS));
        }else{
            snprintf(writeNum, 15, "%d", -(errno));
        }
    }
    /*lock down the errno*/
    pthread_mutex_unlock(&errnoBlock);
    /* send our message to the client */
    send(socket, writeNum , strlen(writeNum) , 0);
    close(socket);
    return;
}





/* this will close a file descriptor or return an error*/
void closeMessage(char * buffer, int socket){
    /* -- message to be parsed --
     * c:fileMode:fd
     */
    /*lock down the errno*/
    pthread_mutex_lock(&errnoBlock);
    /* get the file descriptor we wish to close */
    
    pthread_mutex_lock(&dataLock);   /*lock down the data*/
    int closefd = close(atoi(buffer + 4));
    pthread_mutex_unlock(&dataLock);   /*unlock the data*/
    
    /*build a return message */
    char returnMessage[3];
    
    if(closefd == 0){
        /* we were able to close just fine so return 0 */
        snprintf(returnMessage, 3, "%d", closefd);
        /* remove the fd from our stored list*/
        
        
        /*lock down the data*/
        pthread_mutex_lock(&dataLock);
        closeFd(atoi(buffer + 4));
        /*unlock the data*/
        pthread_mutex_unlock(&dataLock);
        
        
    }else{
        /* we were unable to close so return the errror */
        snprintf(returnMessage, 3, "%d", -(errno));
    }
    /*lock down the errno*/
    pthread_mutex_unlock(&errnoBlock);
    /* send our message back to the client */
    send(socket, returnMessage , strlen(returnMessage) , 0);
    close(socket);
    return;
    
}
/* ----------------------- END OF OPEN - READ - WRITE - CLOSE ----------------------- */







/* ----------------------- THREAD FUNCTIONS ----------------------- */
/* this thread will keep track off its corresponding socket in the queue*/
void * monitorThread(void * qItem){
    
    /* pick apart the queued socket item for its information */
    int socket = ((queue*) qItem) -> socket;
    char * path = ((queue*) qItem) -> path;
    int fileMode = ((queue*) qItem) -> fileMode;
    int flags = ((queue*) qItem) -> flags;
    /*lock down the data*/
    pthread_mutex_lock(&dataLock);
    int request = validRequestGivenMode(path, flags, fileMode);
    /*unlock the data*/
    pthread_mutex_unlock(&dataLock);
    /* while we can not proceed with our desired open */
    while(request == 0){
        
        /* sleep for a second */
        sleep(1);
        
        /*lock down the data*/
        pthread_mutex_lock(&queueLock);
        
        /* check to see if the cleanerThread determined we are waiting in the queue too long */
        if(((queue*) qItem) -> closeDown == 1){
            /*if we are waiting too long we need to close the socket and remove the queued socket from the queue */
            char returnMessage[15];
            snprintf(returnMessage, 15, "%d", -(ETIMEDOUT));
            send(socket, returnMessage , strlen(returnMessage) , 0);
            close(socket);
            
            /*unlock the data*/
            pthread_mutex_unlock(&queueLock);
            
            /*lock down the queue*/
            pthread_mutex_lock(&queueLock);
            removeQueueItem(((queue*) qItem));
            /*unlock the queue*/
            pthread_mutex_unlock(&queueLock);
            
            pthread_exit(&ret1);
            return NULL;
        }else{
            /* we arent told to shut down, so update the time that we have been waiting by 1 second
             * then request to proceed again
             */
            ((queue*) qItem) -> waitTime ++;
            pthread_mutex_lock(&dataLock);
            /*lock the data*/
            request = validRequestGivenMode(path, flags, fileMode);
            /*unlock the data*/
            pthread_mutex_unlock(&dataLock);
            /*unlock the data*/
            pthread_mutex_unlock(&queueLock);
        }
    }
    /* we can proceed with opening our file */
    char * buffer = (char *) malloc(sizeof(char) * strlen(path) + 7);
    char open = 'o';
    char flag = flags + '0';
    /* o:flags:fileMode:path*/
    buffer[0] = open;
    buffer[1] =  buffer[3] = buffer[5] = ':';
    buffer[2] = flag;
    buffer[4] = fileMode + '0';
    strcat(buffer,path);
    /* remove the queued socket from the waiting queue */
    
    pthread_mutex_lock(&queueLock); /*lock the queue*/
    removeQueueItem(((queue*) qItem));
    pthread_mutex_unlock(&queueLock);  /*unlock the queue*/
    
    /* open the file */
    openMessage(buffer,socket);
    pthread_exit(&ret1);
    return NULL;
}







/* remove items from the queue that have been waiting for too long */
void * cleanerThread(){
    
    while(1){
        /* restart from the top of the queue */
        queue * temp = qHead;
        /* traverse the queue while there are items in it */
        while(temp != NULL){
            
            pthread_mutex_lock(&queueLock);/*lock down the data*/
            if(temp!=NULL){
            }
            /* if a socket has been waiting for over 2 seconds get rid of it */
            if(temp != NULL && temp -> waitTime > 2){
                temp -> closeDown = 1;
            }
            if(temp != NULL){
                temp = temp -> next;
            }
            pthread_mutex_unlock(&queueLock); /*unlock the data*/
        }
        sleep(3);
    }
    return NULL;
}






/* this thread will take the message from the client and distributed it to where it needsto go */
void * worker_Thread(void * new_socket){
    
    int count;
    ioctl(*((int*)new_socket), FIONREAD, &count);
    char * buffer = (char *) malloc(sizeof(char) * count + 1);
    read(*((int*)new_socket) , buffer, count);
    /* self explanitory , checks the starting value of each sockets message and determins where to send the message */
    if(buffer[0] == 'o'){
        
        openMessage(buffer, *((int*)new_socket));
    }
    else if(buffer[0] == 'r'){
        
        readMessage(buffer, *((int*)new_socket));
        
    }else if(buffer[0] == 'w'){
        
        writeMessage(buffer, *((int*)new_socket));
        
    }else if(buffer[0] == 'c'){
        
        closeMessage(buffer, *((int*)new_socket));
    }else {
        
        pthread_exit(&ret1);
    }
    pthread_exit(&ret2);
    return NULL;
}
/* ----------------------- END OF THREAD FUNCTIONS ----------------------- */







/* ---------------- SERVER INIT  ---------------- */
int main(int argc, char const *argv[]){
    
    signal(SIGPIPE, SIG_IGN);
    
    if (pthread_mutex_init(&dataLock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return -1;
    }
    if (pthread_mutex_init(&queueLock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return -1;
    }
    if (pthread_mutex_init(&errnoBlock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return -1;
    }
    
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(52678);
    
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    listen(server_fd, 10);
    
    pthread_t cleanerThreadStart;
    if(pthread_create(&cleanerThreadStart, NULL, cleanerThread, NULL) !=0) {
        printf("Error creating thread\n");
        return 0;
    }
    
    while(1){
        /* if we could not accept a new socket return -1 */
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0){
            return -1;
            
        }else{
            /* create our working thread for each new socket */
            pthread_t workerThread;
            if(pthread_create(&workerThread, NULL, worker_Thread, &new_socket) !=0) {
                printf("Error creating thread\n");
                return 0;
            }
        }
    }
    printf("END\n");
    return 0;
}
/* ---------------- END OF SERVER INIT  ---------------- */


