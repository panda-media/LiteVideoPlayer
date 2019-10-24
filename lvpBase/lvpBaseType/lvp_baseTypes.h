#ifndef LVP_BASE_TYPES_H_
#define LVP_BASE_TYPES_H_

#define LVP_BOOL int
#define LVP_FALSE 0
#define LVP_TRUE 1
#define LVP_NULL ((void *)0)

typedef struct LvpMutex
{
    void *ptr;
}LvpMutexRef;


#endif // !LVP_BASE_TYPES_H_

