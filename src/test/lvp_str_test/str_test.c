#include "../testhelp.h"
#include "../../core/lvp_str.h"

int main(){
    const char *src = "abcd123";
    const char *src2 = "abcd";

    char *dst = NULL;
    int ret =  lvp_str_dump(src,&dst);
    test_cond("lvp dump str ",ret == LVP_OK && dst != NULL &&!strcmp(dst,src));
    lvp_mem_free(dst);
    dst = NULL;
    ret = lvp_str_dump_with_size(src,&dst,10);
    test_cond("lvp dump str with size 10",ret != LVP_OK && dst == NULL);

    ret = lvp_str_dump_with_size(src,&dst,strlen(src2)); 
    test_cond("lvp dum str with size sizeof(src2)",ret == LVP_OK && dst != NULL && !strcmp(dst,src2));
    lvp_mem_free(dst);

    return 0;
}