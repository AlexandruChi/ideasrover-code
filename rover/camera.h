#ifndef __camrea_h
#define __camera_h

#include "../cameradata.h"

struct CameraThreadData {
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    _Bool running;
    struct CameraData cameraData;
};



#endif