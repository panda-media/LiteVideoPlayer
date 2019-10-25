#include "lvp_queue.h"

void lvp_queue_add_cap(LVPQueue* queue)
{
#define Queue_Default_Cap 8

    int64_t newCap = 0;
    if (queue->cap< Queue_Default_Cap)
    {
        newCap = Queue_Default_Cap;
    }
    else
    {
        newCap = queue->cap * 2;
    }

    void **datas = (void**)malloc(sizeof(void*)*newCap);
    void **usrDatas = (void**)malloc(sizeof(void*)*newCap);
    lvp_custom_free *freeFuncs = (lvp_custom_free*)malloc(sizeof(lvp_custom_free)*newCap);
    LVP_BOOL *needFrees = (LVP_BOOL*)malloc(sizeof(LVP_BOOL)*newCap);

    for (int64_t i = queue->front; i < queue->back; i++)
    {

    }
}

void lvp_queue_move_data(LVPQueue* queue)
{

}

LVPQueue * lvp_queue_create()
{
    LVPQueue *queue = (LVPQueue*)malloc(sizeof(LVPQueue));

    queue->data = LVP_LP_NULL;
    queue->usrData = LVP_LP_NULL;
    queue->freeFuncs = LVP_NULL;
    queue->needFrees = LVP_NULL;

    queue->cap = 0;
    queue->front = 0;
    queue->back = 0;

    lvp_queue_add_cap(queue);

    return queue;
}

LVP_BOOL lvp_queue_free(LVPQueue * queue)
{
    return LVP_BOOL();
}

LVP_BOOL lvp_queue_clear(LVPQueue * queue)
{
    return LVP_BOOL();
}

LVP_BOOL lvp_queue_push(LVPQueue * queue, void * data, void * usrData, lvp_custom_free freeFunc, LVP_BOOL needFree)
{
    return LVP_BOOL();
}

LVP_BOOL lvp_queue_push_raw(LVPQueue * queue, void * data)
{
    return lvp_queue_push(queue, data, LVP_NULL, LVP_NULL, LVP_FALSE);
}

LVP_BOOL lvp_queue_pop(LVPQueue * queue)
{
    return LVP_BOOL();
}

LVP_BOOL lvp_queue_size(LVPQueue * queue, int64_t * size)
{
    return LVP_BOOL();
}

LVP_BOOL lvp_queue_front(LVPQueue * queue, void ** data, void ** usrData)
{
    return LVP_BOOL();
}

LVP_BOOL lvp_queue_back(LVPQueue * queue, void ** data, void ** usrData)
{
    return LVP_BOOL();
}
