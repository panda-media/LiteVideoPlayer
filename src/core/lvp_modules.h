#ifndef _LVP_MODULES_H_
#define _LVP_MODULES_H_
#include "lvp_core.h"

extern LVPModule lvp_reader_module;

//all  module 
static LVPModule *LVPModules[]={
    &lvp_reader_module,
};
#endif