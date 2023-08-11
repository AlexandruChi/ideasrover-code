#ifndef __camera_h
#define __camera_h

struct InputThread {
    pthread_t threadID;
    pthread_cond_t cond;
    pthread_mutex_t mutex;

    _Bool input;
    _Bool server;
    _Bool network;

    _Bool running;
    _Bool connected;

    void *socket;
    void *data;
    size_t dataSize;
};

// input.c for documentation

#endif