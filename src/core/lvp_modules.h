#ifndef _LVP_MODULES_H_
#define _LVP_MODULES_H_
#include "lvp_core.h"

extern LVPModule lvp_reader_module;
extern LVPModule lvp_pkt_filter;

//all  module 
static LVPModule *LVPModules[]={
    &lvp_reader_module,
    &lvp_pkt_filter,
};
#endif