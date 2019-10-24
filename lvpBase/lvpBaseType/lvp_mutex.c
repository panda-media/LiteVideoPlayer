#include "lvp_mutex.h"
#include "lvp_platform.h"

#ifdef WINDOWS_SYSTEM
#include <Windows.h>

LvpMutexRef lvp_mutex_create()
{
    LvpMutexRef mutex;
    mutex.ptr = CreateSemaphoreA(NULL, 0, 1, "");
    return mutex;
}

LVP_BOOL lvp_mutex_close(LvpMutexRef mutex)
{
    if (mutex.ptr == LVP_NULL)
    {
        return LVP_FALSE;
    }
    LVP_BOOL ret = CloseHandle(mutex.ptr);
    mutex.ptr = LVP_NULL;
    return ret;
}

LVP_BOOL lvp_mutex_lock(LvpMutexRef mutex)
{
    if (mutex.ptr == LVP_NULL)
    {
        return LVP_FALSE;
    }
    return WAIT_OBJECT_0 == WaitForSingleObject(mutex.ptr, INFINITE);
}

LVP_BOOL lvp_mutex_try_lock(LvpMutexRef mutex)
{
    if (mutex.ptr == LVP_NULL)
    {
        return LVP_FALSE;
    }
    return WAIT_OBJECT_0 == WaitForSingleObject(mutex.ptr, 0);
}

LVP_BOOL lvp_mutex_unlock(LvpMutexRef mutex)
{
    if (mutex.ptr == LVP_NULL)
    {
        return LVP_FALSE;
    }
    return ReleaseMutex(mutex.ptr);
}

#else
#ifdef DEBUG

#endif // DEBUG

#endif // WINDOWS_SYSTEM


