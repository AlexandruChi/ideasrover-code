#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Functions that use Connection data type are the only ones suposed to be used in the program */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "input.h"

#ifndef and
#define and &&
#endif // and

#ifndef or
#define or ||
#endif // or

#ifndef false
#ifdef true
#define false !true
#else
#define false NULL
#endif // true
#endif // false

#ifndef true
#define true !false
#endif // true

#ifndef loop
#define loop for(;;)
#endif // loop

// TODO rename input thread to connection thread

// TODO modify thread to run continously and connect to any avalible devices

// TODO create fucntions for creating connections
// TODO create Connection data type

// TODO add errors

// TODO fix documetation
// TODO fix header file

// TODO create python suport
// TODO create c++ suport

// TODO stop ading TODOs

struct ConnectionThread {
    pthread_t threadID;
    pthread_cond_t cond;
    pthread_mutex_t mutex;

    _Bool input;
    _Bool server;
    _Bool network;

    _Bool running;
    _Bool connected;

    void *socketData;
    void *data;
    size_t dataSize;
};

// input treads continuasly colects data from device
// when data is recived it is copied from local memory to shered memory
void *inputThreadMain(void *arg) {
    struct ConnectionThread *inputThread = (struct ConnectionThread*)arg;

    // allocate memory
    pthread_mutex_lock(&inputThread->mutex);
    size_t recvDataSize = inputThread->dataSize;
    pthread_mutex_unlock(&inputThread->mutex);
    void *recvData = malloc(recvDataSize);
    if (!recvData) {
        exit(1);
    }

    // // create server socket
    // int socketfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    // struct sockaddr_un sockaddr;
    // sockaddr.sun_family = AF_LOCAL;
    // pthread_mutex_lock(&inputThread->mutex);
    // strcpy(sockaddr.sun_path, (char*)inputThread->socketData);
    // pthread_mutex_unlock(&inputThread->mutex);
    // bind(socketfd, (struct sockaddr*)&(sockaddr), sizeof(sockaddr));
    // listen(socketfd, 0);
    // accept(socketfd, NULL, NULL);

    pthread_mutex_lock(&inputThread->mutex);
    long long sockets = createConnectionThreadSocket(inputThread->socketData, inputThread->server, inputThread->network);
    inputThread->connected = 1;
    pthread_mutex_unlock(&inputThread->mutex);

    int socketfd, serverfd;
    memcpy(&socketfd, &sockets, sizeof(int));
    memcpy(&serverfd, &sockets + sizeof(int), sizeof(int));

    // main loop
    loop {
        pthread_mutex_lock(&inputThread->mutex);
        if (!inputThread->running) {
            pthread_mutex_unlock(&inputThread->mutex);
            break;
        }
        pthread_mutex_unlock(&inputThread->mutex);
        recv(socketfd, recvData, recvDataSize, 0);
        pthread_mutex_lock(&inputThread->mutex);
        memcpy(inputThread->data, recvData, inputThread->dataSize);
        pthread_mutex_unlock(&inputThread->mutex);
    }

    free(recvData);
    close(socketfd);

    pthread_mutex_lock(&inputThread->mutex);
    if (inputThread->network) {
        close(serverfd);
    }
    inputThread->connected = 0;
    pthread_mutex_unlock(&inputThread->mutex);

    return NULL;
}

// TODO check if it works

// returns the file descriptor for the client
// function return a 8 byte number where the first 4 bytes is the file descriptor of the client
// the last 4 bytes of the number reprezent the server file descriptor
long long createConnectionThreadSocket(const void* socketData, _Bool server, _Bool network) {
    long long socketfd = socket((network ? AF_INET : AF_LOCAL), SOCK_STREAM, 0);
    
    // convert socketData to sockaddr
    struct sockaddr *socketaddr;
    if (network) {
        ((struct sockaddr_in*)socketaddr)->sin_family = AF_INET;
        if (!strlen(socketData + sizeof(in_port_t)) and server) {
            ((struct sockaddr_in*)socketaddr)->sin_addr.s_addr = INADDR_ANY;
        } else {
            ((struct sockaddr_in*)socketaddr)->sin_addr.s_addr = inet_addr(socketData + sizeof(in_port_t));
        }
        ((struct sockaddr_in*)socketaddr)->sin_port = htons(*((in_port_t*)socketData));
    } else {
        ((struct sockaddr_un*)socketaddr)->sun_family = AF_LOCAL;
        strcpy(((struct sockaddr_un*)socketaddr)->sun_path, socketData);
    }

    if (server) {
        bind(socketfd, socketaddr, sizeof(*socketaddr));
        listen(socketfd, 0);
        memcpy(&socketfd + sizeof(int), &socketfd, sizeof(int));
        *((int*)&socketfd) = accept(socketfd, NULL, NULL);
    } else {
        connect(socketfd, socketaddr, sizeof(*socketaddr));
    }

    return socketfd;
}

// creates and starts the connection thread and socket
// dataSize reprezents the maximum size of the data it can revice
// suports AF_LOCAL/AF_UNIX (is network is false) and AF_INET (if network is true)
// const void* socket points to file name or host name depending on the type of socket
// if host name os used for the soket the first 2 bytes reprezent the port (in_port_t)
// for server the host is the adress on witch the server listens for connesction
// to lisne to any connection the host should be set to a empty string
struct ConnectionThread *createConnectionThread(const void *socketData, size_t dataSize, _Bool input, _Bool network, _Bool server) {
    struct ConnectionThread *inputThread = (struct ConnectionThread*)malloc(sizeof(struct ConnectionThread));
    if (!inputThread) {
        exit(1);
    }
    
    inputThread->running = 1;
    inputThread->connected = 0;
    inputThread->dataSize = dataSize;
    inputThread->server = server;
    inputThread->network = network;
    inputThread->input = input;

    if (!pthread_mutex_init(&inputThread->mutex, NULL)) {
        exit(1);
    }
    if (!pthread_cond_init(&inputThread->cond, NULL)) {
        exit(1);
    }

    size_t socketSize;
    if (network) {
        socketSize = sizeof(in_port_t);
        socketSize += strlen(socketData + socketSize);
    } else {
        socketSize = strlen(socketData);
    }

    if (!(inputThread->socketData = malloc(socketSize))) {
        exit(1);
    }

    memcpy(inputThread->socketData, socketData, socketSize);

    if (!(inputThread->data = malloc(dataSize))) {
        exit(1);
    }

    if (!pthread_create(&inputThread->threadID, NULL, inputThreadMain, inputThread)) {
        exit(1);
    }

    return inputThread;
}

// sends stop comand to thread and waits for thread to finish
// dealocates all thread allocated memory
// dealocates connection variable and set it NULL
void destroyConnection(Connection *connection) {
    struct ConnectionThread *connectionThread = (struct ConnectionThread*)(*connection);
    pthread_mutex_lock(&connectionThread->mutex);
    connectionThread->running = 0;
    pthread_mutex_unlock(&connectionThread->mutex);

    pthread_join(connectionThread->threadID, NULL);
    pthread_mutex_destroy(&connectionThread->mutex);
    pthread_cond_destroy(&connectionThread->cond);
    
    free(connectionThread->socketData);
    connectionThread->socketData = NULL;
    free(connectionThread->data);
    connectionThread->data = NULL;

    free(*connection);
    *connection = NULL;
}

// copies the data from the thread memory into caller thread memory
// local copy is crated to alow data to be used while the thread receves new data
// dest should be the same type as the data set when creating the thread
void getConnectionData(const Connection connection, void *dest) {
    struct ConnectionThread *connectionThread = (struct ConnectionThread*)connection;
    pthread_mutex_lock(&connectionThread->mutex);
    memcpy(dest, connectionThread->data, connectionThread->dataSize);
    pthread_mutex_unlock(&connectionThread->mutex);
}

// copies the data from the caller thread memory into thread memory
// local copy is crated to alow caller thread to continue execution while the thread sends data
// src should be the same type as the data set when creating the thread
void setConnectionData(const Connection connection, void *src) {
    struct ConnectionThread *connectionThread = (struct ConnectionThread*)connection;
    pthread_mutex_lock(&connectionThread->mutex);
    memcpy(connectionThread->data, src, connectionThread->dataSize);
    pthread_mutex_unlock(&connectionThread->mutex);
}