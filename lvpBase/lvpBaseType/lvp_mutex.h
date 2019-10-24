#ifndef LVP_MUTEX_H_
#define LVP_MUTEX_H_
#include "lvp_baseTypes.h"

LvpMutexRef lvp_mutex_create();
LVP_BOOL lvp_mutex_close(LvpMutexRef mutex);
LVP_BOOL lvp_mutex_lock(LvpMutexRef mutex);
LVP_BOOL lvp_mutex_try_lock(LvpMutexRef mutex);
LVP_BOOL lvp_mutex_unlock(LvpMutexRef mutex);

#endif // !LVP_MUTEX_H_




