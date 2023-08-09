#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

#include "camera.h"

#define CAMERA_SOCKET_PATH "cameasocket"
#define GET_CAMERA_DATA_COMMAND "getdata"

// TODO add errors

void *cameraThread(void *arg) {
    struct CameraThreadData *threadData = (struct CameraThreadData*)arg;
    char getData[] = GET_CAMERA_DATA_COMMAND;

    pthread_mutex_lock(&threadData->mutex);
    if (threadData->running) {
        pthread_mutex_unlock(&threadData->mutex);
        return;
    }
    pthread_mutex_unlock(&threadData->mutex);

    // create client socket
    int socketfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    struct sockaddr_un sockaddr;
    sockaddr.sun_family = AF_LOCAL;
    strcpy(sockaddr.sun_path, CAMERA_SOCKET_PATH);
    connect(socketfd, (struct sockaddr*)&(sockaddr), sizeof(sockaddr));

    // main loop
    for(;;) {
        // wait for singnal from main thread after seting the shered variables
        pthread_mutex_lock(&threadData->mutex);
        pthread_cond_wait(&threadData->cond, &threadData->mutex);
        if (!threadData->running) {break;}
        pthread_mutex_unlock(&threadData->mutex);

        // send data to camera
        // can be changed to send any type of data
        send(socketfd, &getData, sizeof(getData), 0);
        
        // recive data from camera
        // can be changed to recv any typed of data
        pthread_mutex_lock(&threadData->mutex);
        recv(socketfd, &threadData->cameraData, sizeof(threadData->cameraData), 0);
        pthread_mutex_unlock(&threadData->mutex);
    }

    // close client socket
    close(socketfd);
}