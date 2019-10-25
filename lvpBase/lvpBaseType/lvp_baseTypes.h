#ifndef LVP_BASE_TYPES_H_
#define LVP_BASE_TYPES_H_

#include <stdint.h>

typedef void(*lvp_custom_free)(void *data,void * usrData);

#define LVP_BOOL int
#define LVP_FALSE 0
#define LVP_TRUE 1
#define LVP_NULL ((void *)0)
#define LVP_LP_NULL ((void **)0)

#define DEFINE_LVP_TYPE(TYPENAME) \
  typedef struct { \
    void *ptr; \
  } TYPENAME;


#endif // !LVP_BASE_TYPES_H_

