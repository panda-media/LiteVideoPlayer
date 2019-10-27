#ifndef _LVP_LOG_H_
#define _LVP_LOG_H_

#include "lvp_core.h"

#define LVP_LOG_ERROR   0
#define LVP_LOG_WARIN   100
#define LVP_LOG_DEBUG   200


typedef struct lvp_log{
    char *ctx_name;
    void *usr_data;
    lvp_custom_log log_call;
}LVPLog;

#define ilvplog(log,level,fmt,file,func,line,...) lvp_print(log,level,file,func,line,fmt,__VA_ARGS__)

#define lvp_debug(log,fmt,...) ilvplog(log,LVP_LOG_DEBUG,fmt,__FILE__,__FUNCTION__,__LINE__,__VA_ARGS__)

#define lvp_waring(log,fmt,...) ilvplog(log,LVP_LOG_WARIN,fmt,__FILE__,__FUNCTION__,__LINE__,__VA_ARGS__)

#define lvp_error(log,fmt,...) ilvplog(log,LVP_LOG_ERROR,fmt,__FILE__,__FUNCTION__,__LINE__,__VA_ARGS__)

void lvp_print(struct lvp_log *log,int level,const char *file,const char *func,int line, const char* fmt, ...);

static inline LVPLog* lvp_log_alloc(const char *name){
    LVPLog *log = (LVPLog*)lvp_mem_mallocz(sizeof(*log));
    if(name){
        lvp_str_dump(name,&log->ctx_name);
    }
    return log;
}

static inline void lvp_log_free(LVPLog *log){
    assert(log);
    if(log->ctx_name){
        lvp_mem_free(log->ctx_name);
    }

    log->log_call = NULL;
    lvp_mem_free(log);

}

#endif