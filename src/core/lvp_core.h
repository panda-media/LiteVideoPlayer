#ifndef _LVP_CORE_H_
#define _LVP_CORE_H_

#include "lvp_config.h"
#include "lvp_define.h"
#include "lvp_error.h"


/**
 *lvp custom free use to some context free self
 *@param data free object 
 *@param usr_data usr data
 */
typedef void(*lvp_custom_free)(void *data,void *usr_data);

#include "lvp_mem.h"

#endif