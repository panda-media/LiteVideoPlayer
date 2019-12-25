#include "lvp_core.h"
#include "lvp.h"

int main() {
	int i = 0;
	do {
		LVPCore* core = lvp_core_alloc();
		lvp_core_set_url(core, "d:/tw.mkv");
		lvp_core_play(core);
		lvp_sleep(100000);
		lvp_core_free(core);
		i++;
	} while (i < 100);
	return 0;
}

int tmain(int argc , char *argv[]){
    printf("%s\n",LVP_VER);
    LVPCore* core = lvp_core_alloc();
    if (argc < 2 || argc > 3){
        printf("use LiteVideoPlayer input.xx options \n");
		lvp_core_set_url(core, "d:/tw.mkv");
    //    return -1;
    }

    if(argc == 3){
	    lvp_core_set_option(core, argv[2]);
    }
	if(argc >1){
		lvp_core_set_url(core, argv[1]);
	}
	lvp_core_play(core);
	lvp_sleep(10);
	lvp_core_free(core);
    return 0;
}