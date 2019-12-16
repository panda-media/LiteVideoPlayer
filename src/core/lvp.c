#include "lvp_core.h"
#include "lvp.h"

int main(int argc , char *argv[]){
    printf("%s\n",LVP_VER);
    if (argc < 2 || argc > 3){
        printf("use LiteVideoPlayer input.xx options \n");
        return -1;
    }

    LVPCore* core = lvp_core_alloc();
    if(argc == 3){
	    lvp_core_set_option(core, argv[2]);
    }
	lvp_core_set_url(core, argv[1]);
	lvp_core_play(core);
    char  quite  = '\0';
    while((quite = getchar()) != 'q');
    return 0;
}