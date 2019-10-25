#include "lvp_mutex.h"
#include "lvp_platform.h"

#ifdef WINDOWS_SYSTEM
#include <Windows.h>

LVPMutex lvp_mutex_create()
{
    LVPMutex mutex;
    mutex.ptr = CreateSemaphoreA(NULL, 1, 1, LVP_NULL);
    return mutex;
}

LVP_BOOL lvp_mutex_free(LVPMutex mutex)
{
    if (mutex.ptr == LVP_NULL)
    {
        return LVP_FALSE;
    }
    LVP_BOOL ret = CloseHandle(mutex.ptr);
    mutex.ptr = LVP_NULL;
    return ret;
}

LVP_BOOL lvp_mutex_lock(LVPMutex mutex)
{
    if (mutex.ptr == LVP_NULL)
    {
        return LVP_FALSE;
    }
    return WAIT_OBJECT_0 == WaitForSingleObject(mutex.ptr, INFINITE) ? LVP_TRUE : LVP_FALSE;
}

LVP_BOOL lvp_mutex_try_lock(LVPMutex mutex)
{
    if (mutex.ptr == LVP_NULL)
    {
        return LVP_FALSE;
    }
    return WAIT_OBJECT_0 == WaitForSingleObject(mutex.ptr, 0) ? LVP_TRUE : LVP_FALSE;
}

LVP_BOOL lvp_mutex_unlock(LVPMutex mutex)
{
    if (mutex.ptr == LVP_NULL)
    {
        return LVP_FALSE;
    }

    return 0 != ReleaseSemaphore(mutex.ptr, 1, LVP_NULL) ? LVP_TRUE : LVP_FALSE;
}


#endif // WINDOWS_SYSTEM

#ifdef __linux__

#include <semaphore.h>

LVPMutex lvp_mutex_create()
{
    LVPMutex mutex;
    mutex.ptr = malloc(sizeof(sem_t));
    if (0 == sem_init(mutex.ptr, 0, 1))
    {
        return LVP_TRUE;
    }

    free(mutex.ptr);
    mutex.ptr = LVP_NULL;

    return LVP_FALSE;
}

LVP_BOOL lvp_mutex_free(LVPMutex mutex)
{
    if (mutex.ptr == LVP_NULL)
    {
        return LVP_FALSE;
    }

    if (0 == sem_destroy(mutex.ptr))
    {
        free(mutex.ptr);
        mutex.ptr = LVP_NULL;
        return LVP_TRUE;
    }

    return LVP_FALSE;
}

LVP_BOOL lvp_mutex_lock(LVPMutex mutex)
{
    if (mutex.ptr == LVP_NULL)
    {
        return LVP_FALSE;
    }
    return 0 == sem_wait(mutex.ptr) ? LVP_TRUE : LVP_FALSE;
}

LVP_BOOL lvp_mutex_try_lock(LVPMutex mutex)
{
    if (mutex.ptr == LVP_NULL)
    {
        return LVP_FALSE;
    }

    return 0 == sem_trywait(mutex.ptr) ? LVP_TRUE : LVP_FALSE;
}

LVP_BOOL lvp_mutex_unlock(LVPMutex mutex)
{
    if (mutex.ptr == LVP_NULL)
    {
        return LVP_FALSE;
    }
    return 0 == sem_post(mutex.ptr) ? LVP_TRUE : LVP_FALSE;
}
#endif // __linux__



