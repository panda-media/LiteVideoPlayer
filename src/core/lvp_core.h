#ifndef _LVP_CORE_H_
#define _LVP_CORE_H_

#include "lvp_config.h"
#include "lvp_define.h"
#include "lvp_error.h"


/**
 *lvp custom lvp_mem_free use to some context lvp_mem_free self
 *@param data lvp_mem_free object 
 *@param usr_data usr data
 */
typedef void(*lvp_custom_free)(void *data,void *usr_data);


#include "lvp_mem.h"
#include "lvp_str.h"
#include "lvp_list.h"
#include "lvp_map.h"

#endif
