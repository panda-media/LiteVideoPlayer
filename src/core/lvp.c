#include "lvp_core.h"
#include "lvp.h"
#include <SDL2/SDL.h>

int main(int argc , char *argv[]){
	int j = 2;
	while (j>0) {
		//j--;
		printf("%s\n", LVP_VER);
		LVPCore* core = lvp_core_alloc();
		if (argc < 2 || argc > 3) {
			printf("use LiteVideoPlayer input.xx options \n");
			lvp_core_set_url(core, "c:/oupt/oupt.mkv");
		//	lvp_core_set_option(core, "-lvp_vcodec h264_cuvid");
			//    return -1;
		}

		if (argc == 3) {
			lvp_core_set_option(core, argv[2]);
		}
		if (argc > 1) {
			lvp_core_set_url(core, argv[1]);
		}
		lvp_core_play(core);
		int i = 0;
		while (i<1000)
		{
			SDL_Event ev;
			while (SDL_PollEvent(&ev));
			if (ev.type == SDL_QUIT) {
				break;
			}
			lvp_sleep(10);
			//i++;
			
		}
		lvp_core_free(core);
		SDL_Quit();
	}
    return 0;
}