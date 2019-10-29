#ifndef _LVP_MODULE_H_
#define _LVP_MODULE_H_

#include "lvp_core.h"

typedef enum lvp_module_type{
    LVP_MODULE_CORE,
    LVP_MODULE_READER,
    LVP_MODULE_PKT_FILTER,
    LVP_MODULE_DECODER,
    LVP_MODULE_FRAME_FILTER,
    LVP_MODULE_RENDER
}LVPModuleType;

typedef struct lvp_module{
    uint64_t        version;
    char            *name;
    LVPModuleType   type;
    int             private_data_size;
    void            *private_data;

    int             (*module_init)(struct lvp_module *module, 
                                    LVPMap *options,
                                    LVPEventControl *ctl,
                                    lvp_custom_log clog);
    void            (*module_close)(struct lvp_module *module);

}LVPModule;


//all  module 
static LVPModule *LVPModules[]={
};

#endif