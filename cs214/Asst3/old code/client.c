#include "client.h"

int fileMode;
char * hostIP;
int initiated;

/* checks to see if a host exists, and gets its IP address, and users desired fileMode*/
int netserverinit(char * hostname, int filemode){
    
    if(filemode < 0 || filemode > 2){
        
        errno = INVALID_FILE_MODE;
        return -1;
    }
    /* check to see if a host with this name exists */
    struct hostent * host;
    host = gethostbyname(hostname);
    
    if(host == NULL){
        
        errno = HOST_NOT_FOUND;
        return -1;
    }
    /* get the IP address of the host */
    if(inet_ntoa( *( struct in_addr*)( host -> h_addr_list[0])) != NULL) {
        
        hostIP = malloc(sizeof(char) * strlen(inet_ntoa( *( struct in_addr*)( host -> h_addr_list[0]))) + 1);
        strcpy(hostIP,inet_ntoa( *( struct in_addr*)( host -> h_addr_list[0])));
        
    }else{
        
        errno = HOST_NOT_FOUND;
        return -1;
    }
    
    fileMode = filemode;
    initiated = 1;
    return 0;
}

/* corresponds to sendCode 0 on server side - attemps to get a file descriptor from a file path */
int netopen(const char *pathname, int flags){
    
    if(!initiated){
        
        errno = HOST_NOT_FOUND;
        return -1;
    }
    
    if(flags < 0){
        /* need error for wrong flags */
        return -1;
    }
    
    char * open = "open";
    /* build the info packet to send to the connectServer function */
    packet * newPacket = (packet *) malloc(sizeof(packet));
    newPacket->objective = (char *) malloc(sizeof(char) * strlen(open) + 1);
    strcpy(newPacket->objective, open);
    newPacket->message = (char *) malloc(sizeof(char) * strlen(pathname) + 4);
    strcat(newPacket->message, pathname);
    strcat(newPacket->message, ",");
    char intToChar[2];
    sprintf(intToChar, "%d", flags);
    strcat(newPacket->message, intToChar);
    newPacket->fileMode = fileMode;
    newPacket->size = 0;
    /* connect to the server */
    connectServer(newPacket);
    /* return value is sent to the packet from the server */
    int returnValue;
    if(newPacket->result[0] == '-'){
        
        returnValue = -(atoi(newPacket->result + 1));
    }else{
        returnValue = -(atoi(newPacket->result));
        
    }
    
    /* free the packet */
    free(newPacket->objective);
    free(newPacket->message);
    free(newPacket->result);
    free(newPacket);
    /* return file descriptor or error */
    return returnValue;
    
}
/* reads a desired amount of bytes from a file descriptor */
ssize_t netread(int fildes, void *buf, size_t nbyte){
    
    if(!initiated){
        
        errno = HOST_NOT_FOUND;
        return -1;
    }
    
    if(fildes == -1 || fildes >= 0 || nbyte <=0){
        
        errno = EBADF;
        return -1;
        
    }else{
        
        fildes = -(fildes);
    }
     char * read = "read";
    packet * newPacket = (packet *) malloc(sizeof(packet));
    newPacket->objective = (char *) malloc(sizeof(char) * strlen(read) + 1);
    strcpy(newPacket->objective, "read");
    newPacket->message = (char *) malloc(2);
    strcpy(newPacket->message," ");
    newPacket->fileMode = fileMode;
    newPacket->fd = fildes;
    newPacket->size = nbyte;

    connectServer(newPacket);
    int returnValue;
    
    if(newPacket->result[0] == '-'){
        
        returnValue = -(atoi(newPacket->result + 1));
    }else{
        
        returnValue = atoi(newPacket->result);
    }
    
    
    printf("\n%s\n", newPacket->result);
    /*int i = 0;
    
    while(*((char*)(buf + i)) != ','){
        
        i++;
    }
    printf("%s\n", ((char*)(buf+i+1)));
    */
    free(newPacket->objective);
    free(newPacket->message);
    free(newPacket->result);
    free(newPacket);
    
    return returnValue;
}
/* attemps to close a file descriptor on the server */
int netclose(int fd){
    
    if(!initiated){
        
        errno = HOST_NOT_FOUND;
        return -1;
    }
    
    if(fd == -1 || fd >= 0){
        
        errno = EBADF;
        return -1;
        
    }else{
        
        fd = -(fd);
    }
    char * close = "close";
    /* builds an info packet to send to connectServer */
    packet * newPacket = (packet *) malloc(sizeof(packet));
    newPacket->objective = (char *) malloc(sizeof(char)* strlen(close) +1);
    strcpy(newPacket->objective, close);
    newPacket->message = (char *) malloc(2);
    strcpy(newPacket->message," ");
    newPacket->fileMode = fileMode;
    newPacket->fd = fd;
    newPacket->size = 0;
    
    connectServer(newPacket);
    int returnValue;
    
    if(newPacket->result[0] == '-'){
        
        returnValue = -(atoi(newPacket->result + 1));
    }else{
        returnValue = -(atoi(newPacket->result));
        
    }
    
    
    free(newPacket->objective);
    free(newPacket->message);
    free(newPacket);
    
    return returnValue;
    
}


/* takes an info packet and sends the information to the server */
int connectServer(packet * newPacket){
    
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char * message = (char *) malloc(sizeof(char)* (strlen(newPacket->objective) + strlen(newPacket->message) + 20));
    /* build a stream to send to the server */
    strcat(message, newPacket->objective);
    strcat(message, ",");
    strcat(message, newPacket->message);
    strcat(message, ",");
    char intToChar[3];
    sprintf(intToChar, "%d", newPacket->fileMode);
    strcat(message,intToChar);
    strcat(message, ",");
    sprintf(intToChar, "%d", newPacket->fd);
    strcat(message, intToChar);
    strcat(message, ",");
    sprintf(intToChar, "%d", newPacket->size);
    strcat(message, intToChar);
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n%s\n", strerror(errno));
        return -1;
    }
    
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(52000);
    
    if(inet_pton(AF_INET, hostIP, &serv_addr.sin_addr)<=0)
    {
        printf("\n%s\n", strerror(errno));
        return -1;
    }
    /* connected to the server */
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n%s\n", strerror(errno));
        return -1;
    }
   /* printf("\n%s\n", (message)); */
    /* send our stream to the server */
    send(sock , message , strlen(message) , 0 );
    /* read the return value from the server and store it in our buffer */
    read(sock , buffer, 1024);
    /* store the buffer value into the info packet */
    newPacket->result = (char *) malloc(sizeof(strlen(buffer) + 1));
    strcpy(newPacket->result, buffer);
    
    return 0;
}
int main(int argc, char *argv[])
{
    
    netserverinit("assembly.cs.rutgers.edu", 1);
    
    
    int fd = netopen("client.c", O_RDONLY);
    fd = netopen("client.c", O_RDONLY);
    fd = netopen("client.c", O_RDONLY);
    fd = netopen("client.c", O_RDONLY);
   fd =  netopen("client.c", O_RDONLY);
   fd =  netopen("client.c", O_RDONLY);
   fd =  netopen("client.c", O_RDONLY);
    
    int y = netclose(fd);
    printf("x %d\n",fd);
    printf("y %d\n",y);
    
    char * buf = malloc(20);
    y = netread(-8,buf,10);
    
    
    
    printf("x %d\n",fd);
    printf("y %d\n",y);
    
    
    
    return 0;
}

