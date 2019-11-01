#ifndef _LVP_MODULE_H_
#define _LVP_MODULE_H_

#include "lvp_core.h"

typedef enum lvp_module_type{
    LVP_MODULE_CORE = 1,
    LVP_MODULE_READER = 2,
    LVP_MODULE_PKT_FILTER = 4,
    LVP_MODULE_DECODER = 8,
    LVP_MODULE_FRAME_FILTER = 0x10,
    LVP_MODULE_RENDER = 0x20,
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
                                    LVPLog *log);
    void            (*module_close)(struct lvp_module *module);

}LVPModule;


#endif