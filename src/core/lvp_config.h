#ifndef _LVP_CONFIG_H_
#define _LVP_CONFIG_H_

#define LVP_LINUX

#ifdef LVP_LINUX
#include "lvp_linux_config.h"
#define LVP_USE_PTHREAD  1

#elif LVP_WIN32
#include "lvp_win32_config.h"

#endif

#endif