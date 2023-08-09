#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "input.h"

// TODO add errors

void *inputThread(void *arg) {
    struct InputThread *inputThread = (struct InputThread*)arg;
    char getDataCommand[] = "get";

    pthread_mutex_lock(&inputThread->mutex);
    if (inputThread->running) {
        pthread_mutex_unlock(&inputThread->mutex);
        return;
    }
    pthread_mutex_unlock(&inputThread->mutex);

    // create client socket
    int socketfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    struct sockaddr_un sockaddr;
    sockaddr.sun_family = AF_LOCAL;
    pthread_mutex_lock(&inputThread->mutex);
    strcpy(sockaddr.sun_path, inputThread->socketPath);
    pthread_mutex_unlock(&inputThread->mutex);
    connect(socketfd, (struct sockaddr*)&(sockaddr), sizeof(sockaddr));

    // main loop
    for(;;) {
        pthread_mutex_lock(&inputThread->mutex);
        pthread_cond_wait(&inputThread->cond, &inputThread->mutex);
        if (!inputThread->running) {
            pthread_mutex_unlock(&inputThread->mutex);
            break;
        }
        pthread_mutex_unlock(&inputThread->mutex);
        send(socketfd, &getDataCommand, sizeof(getDataCommand), 0);
        pthread_mutex_lock(&inputThread->mutex);
        recv(socketfd, inputThread->data, inputThread->dataSize, 0);
        pthread_mutex_unlock(&inputThread->mutex);
    }

    // close client socket
    close(socketfd);
}

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
    inputThread->threadID = pthread_create(&inputThread->threadID, NULL, inputThread, inputThread);
    return inputThread;
}

void destroyInputThread(struct InputThread *inputThread) {
    pthread_mutex_lock(&inputThread->mutex);inputThread->running = 0;
    inputThread->running = 0;
    pthread_cond_signal(&inputThread->cond);
    pthread_mutex_unlock(&inputThread->mutex);
    pthread_join(inputThread->threadID, NULL);
    pthread_mutex_destroy(&inputThread->mutex);
    pthread_cond_destroy(&inputThread->cond);
}

void *getInputData(struct InputThread *inputThread) {
    void *data = malloc(inputThread->dataSize);
    if (!data) {exit(1);}
    pthread_mutex_lock(&inputThread->mutex);
    pthread_cond_signal(&inputThread->cond);
    memcpy(data, inputThread->data, inputThread->dataSize);
    pthread_mutex_unlock(&inputThread->mutex);
    return data;
}