#ifndef __camera_h
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

struct InputThread *createInputThread(const char *socketPath, size_t dataSize);
void destroyInputThread(struct InputThread *inputThread);
void copyInputThreadData(struct InputThread *inputThread, void *dest);
#endif