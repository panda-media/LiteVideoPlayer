#include "lvp_module.h"
#include "lvp_modules.h"

LVPModule* lvp_module_get_module(const char *module_name, LVPModuleType type){
    size_t size = sizeof(LVPModules)/sizeof(LVPModules[0]);
    LVPModule *m = NULL;
    for (size_t i = 0; i < size; i++)
    {
        m = LVPModules[i];
        if(type == m->type){
            if(module_name&&!strcmp(module_name,m->name)){
                break;
            }else{
                if(module_name){
                    m = NULL;
                    continue;
                }else{
                    break;
                }

            }
        }
        m = NULL;
    }
    if(!m){
        return NULL;
    }
    LVPModule *ret = (LVPModule*)lvp_mem_mallocz(sizeof(*ret));
    memcpy(ret,m,sizeof(*ret));
    return ret;
}

int lvp_module_init(LVPModule *module,LVPMap *options, LVPEventControl *ctl, LVPLog *log){
    assert(module);
    assert(options);
    assert(ctl);
    if(!module->module_init){
        lvp_error(NULL,"module %s no function for init ",module->name);
        return LVP_E_FATAL_ERROR;
    }
    module->private_data = lvp_mem_mallocz(module->private_data_size);
    int ret = module->module_init(module,options,ctl,log);
    if(ret != LVP_OK)
        lvp_waring(NULL,"module %s init return %d",module->name,ret);
    else
        lvp_debug(NULL,"module %s init return %d",module->name,ret);

    return LVP_OK;
}


LVPModule* lvp_module_iterate(void **opaque){
    size_t size = sizeof(LVPModules)/sizeof(LVPModules[0])-1;
    uintptr_t i = (uintptr_t)*opaque;
    LVPModule *m = NULL;
    if(i<=size){
        m = LVPModules[i];
    }
    if(m){
        *opaque = (void*)(i+1);
    }
    return m;
}

LVPModule* lvp_module_create_module(LVPModule *m){
    LVPModule *ret = (LVPModule*)lvp_mem_mallocz(sizeof(*ret));
    memcpy(ret,m,sizeof(*m));
    return ret;
}

void lvp_module_close(LVPModule *m){
    m->module_close(m);
}