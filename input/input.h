#ifndef __camrea_h
#define __camera_h

struct InputThread {
    pthread_t threadID;
    pthread_cond_t cond;
    pthread_mutex_t mutex;

    char *socketPath;
    _Bool running;

    void *data;
    size_t dataSize;
};

#endif