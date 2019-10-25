/*
 * @Description: memory funtion
 * @Author: fgodtdev@hotmail.com
 * @Date: 2019-08-26 09:37:08
 * whole lib is based on ffmpeg so we use av_malloc 
 */
#ifndef _LVP_MEM_H_
#define _LVP_MEM_H_

#include <stdlib.h>
#include <libavutil/mem.h>

/**
 * @description: alloc memory
 * @param t alloc mem size
 * @return  NULL for error
 */
static inline void *lvp_mem_malloc(size_t t){
    return av_malloc(t);
}

/**
 * @description: alloc memory and set all zero
 * @param t alloc mem size
 * @return NULL for error
 */
static inline void *lvp_mem_mallocz(size_t t){
    return av_mallocz(t);
}

/**
 * @description: free memory
 * @param data free data
 */
static inline void lvp_mem_free(void *data){
    av_free(data);
}

#endif