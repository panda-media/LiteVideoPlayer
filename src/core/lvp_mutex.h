#ifndef LVP_MUTEX_H_
#define LVP_MUTEX_H_
#include "lvp_core.h"

typedef struct 
{
    void *ptr;
}LVPMutex;


LVPMutex lvp_mutex_create();
LVP_BOOL lvp_mutex_free(LVPMutex mutex);
LVP_BOOL lvp_mutex_lock(LVPMutex mutex);
LVP_BOOL lvp_mutex_try_lock(LVPMutex mutex);
LVP_BOOL lvp_mutex_unlock(LVPMutex mutex);

#endif // !LVP_MUTEX_H_




