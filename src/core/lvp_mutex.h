#ifndef LVP_MUTEX_H_
#define LVP_MUTEX_H_
#include "lvp_core.h"

#ifdef LVP_WIN32
typedef CRITICAL_SECTION lvp_mutex;
#endif // LVP_WIN32
#ifdef LVP_LINUX
#include <pthread.h>
typedef pthread_mutex_t lvp_mutex;
#endif // LVP_LINUX


LVP_BOOL lvp_mutex_create(lvp_mutex* mutex);
LVP_BOOL lvp_mutex_free(lvp_mutex* mutex);
LVP_BOOL lvp_mutex_lock(lvp_mutex* mutex);
LVP_BOOL lvp_mutex_try_lock(lvp_mutex* mutex);
LVP_BOOL lvp_mutex_unlock(lvp_mutex* mutex);

#endif // !LVP_MUTEX_H_




