#include "lvp_nqueue.h"

LVPNQueue *lvp_nqueue_alloc(int cap){
    LVPNQueue *q = (LVPNQueue*)lvp_mem_mallocz(sizeof(*q));
    q->cap = cap;
    q->datas = lvp_mem_mallocz(sizeof(LVPQueueEntry)*cap);
    return q;
}

void lvp_nqueue_free(LVPNQueue *q){
    for (size_t i = 0; i < q->size; i++)
    {
        if (q->rpos == q->cap)
        {
            q->rpos = 0;
        }

        LVPQueueEntry *entry = q->datas[q->rpos];
        if(entry->need_free == LVP_TRUE){
            if(entry->free){
                entry->free(entry->data,entry->usr_data);
            }else
            {
                lvp_mem_free(entry->data);
            }
        }
    }

    for (size_t i = 0; i < q->cap; i++)
    {
        lvp_mem_free(q->datas[i]);
    }

    lvp_mem_free(q);
    
}

int lvp_nqueue_push(LVPNQueue *q, void *data,void *usr_data, lvp_custom_free free,LVP_BOOL need_free){
    if(q->size==q->cap){
        return LVP_E_NO_MEM;
    }
    LVPQueueEntry *entry = q->datas[q->wpos];
    entry->data = data;
    entry->usr_data = usr_data;
    entry->need_free = need_free;
    entry->free = free;
    q->wpos++;
    q->size++;
    return LVP_OK;
}

int lvp_nqueue_pop(LVPNQueue *q, void **data){
    if(q->size==0){
        return LVP_E_NO_MEDIA;
    }
    LVPQueueEntry *entry = q->datas[q->rpos];
    void *p = *data;
    p = entry->data;
    q->rpos--;
    q->size--;
    return LVP_OK;
}
