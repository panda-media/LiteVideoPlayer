#ifndef _LVP_STR_H_
#define _LVP_STR_H_

#include "lvp_core.h"

static inline int lvp_str_dump_with_size(const char *src, char **dst,size_t size){
    *dst = (char*)lvp_mem_mallocz(size+1);
    if(*dst == NULL){
        return LVP_E_NO_MEM;
    }
    if(strlen(src) < size){
        lvp_mem_free(*dst);
        *dst = NULL;
        return LVP_E_NO_MEDIA;
    }
    char *tmp = (char*)*dst;
    memcpy(tmp,src,size);
    return LVP_OK;
}

static inline int lvp_str_dump(const char *src, char **dst){
    int len = strlen(src);
    return lvp_str_dump_with_size(src,dst,len);
}

#endif