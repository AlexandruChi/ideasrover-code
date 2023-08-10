#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "input.h"

// TODO add errors

// input treads continuasly colects data from device
// when data is recived it is copied drom local memory to shered memory
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
    strcpy(sockaddr.sun_path, inputThread->socketPath);
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

// creates and starts the input device thread on socket socketPath
// dataSize reprezents the maximum size of the data it can revice
struct InputThread *createInputThread(const char *socketPath, size_t dataSize) {
    struct InputThread *inputThread = (struct InputThread*)malloc(sizeof(struct InputThread));
    if (!inputThread) {exit(1);}
    inputThread->running = 1;
    inputThread->dataSize = dataSize;
    if (!pthread_mutex_init(&inputThread->mutex, NULL)) {exit(1);}
    if (!pthread_cond_init(&inputThread->cond, NULL)) {exit(1);}
    inputThread->socketPath = (char*)malloc(strlen(socketPath));
    if (!inputThread->socketPath) {exit(1);}
    inputThread->data = malloc(dataSize);
    if (!inputThread->data) {exit(1);}
    strcpy(inputThread->socketPath, socketPath);
    if (!pthread_create(&inputThread->threadID, NULL, inputThreadMain, inputThread)) {exit(1);}
    return inputThread;
}

// sends stop comand to thread and waits for thread to finish
// dealocates all thread allocated memory
void destroyInputThread(struct InputThread *inputThread) {
    pthread_mutex_lock(&inputThread->mutex);
    inputThread->running = 0;
    pthread_mutex_unlock(&inputThread->mutex);
    pthread_join(inputThread->threadID, NULL);
    pthread_mutex_destroy(&inputThread->mutex);
    pthread_cond_destroy(&inputThread->cond);
    free(inputThread->socketPath);
    inputThread->socketPath = NULL;
    free(inputThread->data);
    inputThread->data = NULL;
}

// TODO probably dosn't work
// copies the data from the thread memory into caller thread memory
// local copy is crated to alow data to be used whyle the input thread receves new data
void copyInputThreadData(struct InputThread *inputThread, void *dest) {
    pthread_mutex_lock(&inputThread->mutex);
    memcpy(dest, inputThread->data, inputThread->dataSize);
    pthread_mutex_unlock(&inputThread->mutex);
}