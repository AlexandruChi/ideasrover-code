#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "input.h"

// TODO rename input thread to connection thread

// TODO create fucntions for creating connections
// TODO create Connection data type
// TODO create function for creating sockets

// TODO add errors

// TODO fix documetation
// TODO fix header file

// TODO create python suport
// TODO create c++ suport

// TODO stop ading TODOs

// input treads continuasly colects data from device
// when data is recived it is copied from local memory to shered memory
void *inputThreadMain(void *arg) {
    struct InputThread *inputThread = (struct InputThread*)arg;

    // allocate memory
    pthread_mutex_lock(&inputThread->mutex);
    size_t recvDataSize = inputThread->dataSize;
    pthread_mutex_unlock(&inputThread->mutex);
    void *recvData = malloc(recvDataSize);
    if (!recvData) {
        exit(1);
    }

    // create server socket
    int socketfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    struct sockaddr_un sockaddr;
    sockaddr.sun_family = AF_LOCAL;
    pthread_mutex_lock(&inputThread->mutex);
    strcpy(sockaddr.sun_path, (char*)inputThread->socket);
    pthread_mutex_unlock(&inputThread->mutex);
    bind(socketfd, (struct sockaddr*)&(sockaddr), sizeof(sockaddr));
    listen(socketfd, 0);
    accept(socketfd, NULL, NULL);

    // confirm connection to parent thread
    pthread_mutex_lock(&inputThread->mutex);
    pthread_cond_signal(&inputThread->cond);
    pthread_mutex_unlock(&inputThread->mutex);

    // main loop
    for(;;) {
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
    return NULL;
}

int createInputThreadSocket(const void* socket, _Bool server, _Bool network) {
    
}

// creates and starts the connection thread and socket
// dataSize reprezents the maximum size of the data it can revice
// const void* socket points to file name or host name depending on the type of socket
// if host name os used for the soket the first 2 bytes reprezent the port (in_port_t)
struct InputThread *createConnectionThread(const void *socket, size_t dataSize, _Bool input, _Bool network, _Bool server) {
    struct InputThread *inputThread = (struct InputThread*)malloc(sizeof(struct InputThread));
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
        socketSize += strlen(socket + socketSize);
    } else {
        socketSize = strlen(socket);
    }

    if (!(inputThread->socket = malloc(socketSize))) {
        exit(1);
    }

    memcpy(inputThread->socket, socket, socketSize);

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
void destroyConnection(struct InputThread *inputThread) {
    pthread_mutex_lock(&inputThread->mutex);
    inputThread->running = 0;
    pthread_mutex_unlock(&inputThread->mutex);

    pthread_join(inputThread->threadID, NULL);
    pthread_mutex_destroy(&inputThread->mutex);
    pthread_cond_destroy(&inputThread->cond);
    
    free(inputThread->socket);
    inputThread->socket = NULL;
    free(inputThread->data);
    inputThread->data = NULL;
}

// copies the data from the thread memory into caller thread memory
// local copy is crated to alow data to be used whyle the input thread receves new data
void copyConnectionThreadData(struct InputThread *inputThread, void *dest) {
    pthread_mutex_lock(&inputThread->mutex);
    memcpy(dest, inputThread->data, inputThread->dataSize);
    pthread_mutex_unlock(&inputThread->mutex);
}