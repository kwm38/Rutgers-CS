#include "server.h"

fdLL * head;
int fileMode;
int ret1= -1;
int ret2= 0;

/* ----------------  DONE  ----------------
 * rebuilds the info packet from the client stream
 * the client stream originates from an info_packet on the client side
 */
int parseBuffer(int sendCode, char * buffer, packet * new_packet){
    
    int infoCount = 0;
    char * pointer = buffer;
    int sizeOfWord = 0;
    int posInStr = 0;
    while(pointer[0] != '\0'){
        
        while(pointer[0] != ',' && pointer[0] != '\0'){
            
            sizeOfWord++;
            pointer++;
        }
        pointer++;
        
        char * temp = (char*)malloc(sizeof(char) * sizeOfWord + 1);
        strncpy(temp, buffer + posInStr, sizeOfWord);
        temp[sizeOfWord] = '\0';
        posInStr += sizeOfWord + 1;
        sizeOfWord = 0;
        
        if (infoCount == 1){
            
            /* send code is from open */
            if(sendCode == 0){
                
                new_packet->path = malloc(sizeof(char) * strlen(temp) + 1);
                strcpy(new_packet->path, temp);
                
                /* send code is from write */
            }else if(sendCode == 2){
                
                new_packet->message = malloc(sizeof(char) * strlen(temp) + 1);
                strcpy(new_packet->message, temp);
            }else {
                
                new_packet->message = malloc(2);
                strcpy(new_packet->message," ");
            }
            
        }else if (infoCount == 2){
            
            if(pointer[0] == '-'){
                
                return -1;
            }
            
            if(sendCode == 0){
                
                new_packet->flags = atoi(temp);
                
            }else{
                
                new_packet->fileMode = atoi(temp);
            }
            
        }else if (infoCount == 3){
            
            if(pointer[0] == '-'){
                
                return -1;
            }
            
            if(sendCode == 0){
                
                new_packet->fd = 0;
                new_packet->fileMode = atoi(temp);
                new_packet->message = malloc(2);
                strcpy(new_packet->message," ");
                new_packet->size = 0;
                return 0 ;
                
            }else {
                
                new_packet->fd = atoi(temp);
                
            }
            
        }else if (infoCount == 4){
            
            if(pointer[0] == '-'){
                
                return -1;
            }
            
            new_packet->size = atoi(temp);
            return 0;
        }
        infoCount += 1;
    }
    return 0 ;
}

/* ----------------  DONE  ----------------
 * stores our file descriptors in a linked list */
int storefd(int fd){
    
    if (head == NULL){
        
        head = (fdLL *)malloc(sizeof(fdLL));
        head-> fd = fd;
        head-> inUse = 0;
        head->prev = head->next = NULL;
        
        return 1;
        
    }else{
        
        fdLL * temp = head;
        fdLL * prev = head;
        
        while(temp != NULL){
            prev = temp;
            temp = temp->next;
        }
        
        temp = (fdLL *)malloc(sizeof(fdLL));
        temp-> fd = fd;
        temp-> inUse = 0;
        temp->next = NULL;
        temp->prev = prev;
        prev->next = temp;
        
        return 1;
    }
    
    return 0;
}
/* ----------------  DONE  ----------------
 * retreives our file descriptors from a linked list */
fdLL * retrieveFd(int fd){
    
    if (head == NULL){
        
        errno = EACCES;
        return head;
        
    }else{
        
        fdLL * temp = head;
        
        while(temp != NULL){
            
            if(temp->fd == fd){
                
                return temp;
            }
            temp = temp->next;
        }
        
        errno = EACCES;
        return head;
    }
    
    errno = EACCES;
    return head;
}
/* ----------------  DONE  ----------------
 * frees an info packet */
void freeInfoPacket(packet * new_packet){
    
    if(new_packet->path != NULL){
        
        free(new_packet->path);
    }
    if(strcmp(new_packet->message," ") != 0){
        
        free(new_packet->message);
    }
    
    free(new_packet);
    
}

/* ----------------  DONE  ----------------
 * server call to open a file */
int serveropen(int new_socket, char * buffer){
    
    packet * new_packet = (packet *) malloc(sizeof(packet));
    
    parseBuffer(0, buffer, new_packet );
    
    if(new_packet->flags < 0  || new_packet->flags > 2){
        
        errno = EACCES;
        return errno;
    }
    
    int fd = open(new_packet->path, new_packet->flags);
    
    storefd(fd);
    freeInfoPacket(new_packet);
    
    if(fd < 0){
        
        return errno;
    }
    
    return fd;
    
}

int serverread(int new_socket,char * buffer){
    
    packet * new_packet = (packet *) malloc(sizeof(packet));
    
    int outcome = parseBuffer(1, buffer, new_packet);
    
    if(outcome < 0){
        
        return -1;
    }
    
    fdLL* temp = retrieveFd(new_packet->fd);
    
    if(temp == NULL){
        
        return -1;
    }
    
    while(temp->inUse == 1){
        
        /* wait */
    }
    
    int ammount;
    
    if(temp->inUse != 1){
        
        temp->inUse = 1;
        
        memset(&buffer[0], 0, 1024);
        
        ammount = read(temp->fd, buffer, new_packet->size);
        
        temp->inUse = 0;
        
        freeInfoPacket(new_packet);
    }
    
    
    return ammount;
    
}
int serverwrite(int new_socket,char * buffer){
    
    
    return 0;
}
/* ----------------  DONE  ----------------
 * closes a valid and open fd */
int serverclose(int new_socket,char * buffer){
    
    packet * new_packet = (packet *) malloc(sizeof(packet));
    
    int outcome = parseBuffer(4, buffer, new_packet);
    
    if(outcome < 0 || head == NULL){
        
        errno = EBADF;
        return -1;
    }
    
    fdLL * temp = head;
    
    while(temp != NULL){
        
        if(temp->fd == new_packet->fd){
            
            if(temp->prev!=NULL){
                
                temp->prev->next = temp->next;
            }
            if(temp->next!=NULL){
                
                temp->next->prev = temp->prev;
            }
            
            if(temp == head){
                
                head = NULL;
                
            }else{
                
                free(temp);
            }
            close(new_packet->fd);
            freeInfoPacket(new_packet);
            return 0;
        }
        
        temp = temp->next;
    }
    
    return -1;
}

void * worker_Thread(void * new_socket){
    
    char buffer[1024] = {0};
    int valread = read(*((int*)new_socket) , buffer, 1024);
    char * pointer = buffer;
    int count = 0;
    char * objective = (char *) malloc(sizeof(char) * valread + 1);
    
    while(pointer[0] != ','){
        
        count ++;
        pointer ++;
    }
    
    strncpy(objective, buffer , count);
    objective[count] = '\0';
    count = 0;
    /* ----------------  DONE  ----------------*/
    if(strcmp(objective, "open") == 0){
        
        int fd =  serveropen(*((int*)new_socket),buffer);
        char  fdChar[15];
        snprintf(fdChar, 15, "%d", fd);
        send(*((int*)new_socket) , fdChar , strlen(fdChar) , 0);
        close(*((int*)new_socket));
        
    }else if(strcmp(objective, "read") == 0){
        
        int readValue =  serverread(*((int*)new_socket),buffer);
        printf ("read value: %d\n", readValue);
        char * readChar = malloc(15);
        snprintf(readChar, 15, "%d", readValue);
        char * result = (char *) malloc( 7 + strlen(buffer));
        strcat(result,readChar);
        strcat(result,",");
        strcat(result,buffer);
        send(*((int*)new_socket) , result , strlen(result) , 0);
        close(*((int*)new_socket));
        
    }else if(strcmp(objective, "write") == 0){
        
        serverwrite(*((int*)new_socket),buffer);
        
        /* ----------------  DONE  ----------------*/
    }else if(strcmp(objective, "close") == 0){
        
        int outcome = serverclose(*((int*)new_socket),buffer);
        char * result = (char*)malloc(3);
        
        if(outcome < 0){
            
            result[0] = '-';
            result[1] = '1';
            
        }else{
            
            result[0] = '0';
        }
        send(*((int*)new_socket) , result , strlen(result) , 0);
        close(*((int*)new_socket));
        pthread_exit(&ret1);
        
    }else {
        
        pthread_exit(&ret1);
    }
    
    pthread_exit(&ret2);
    return NULL;
}

/* ----------------  DONE  ----------------
 * initializes the server socket and creates threads for new messages */
int main(int argc, char const *argv[]){
    
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
    address.sin_port = htons( PORT );
    
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    listen(server_fd, 10);
    
    while(1){
        
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0){
            
            return -1;
            
        }else{
            
            pthread_t workerThread;
            
            if(pthread_create(&workerThread, NULL, worker_Thread, &new_socket) !=0) {
                
                printf("Error creating thread\n");
                return 0;
            }
        }
    }
    
    return 0;
}


