#include "../../core/lvp_core.h"
#include <SDL2/SDL.h>


int main(int argc, char **argv){

    LVPCore *core = lvp_core_alloc();
    lvp_core_set_option(core,"-readers abcreader -lvphw qsv");
	lvp_core_set_url(core, "d:/test.flv");
    lvp_core_play(core);
	SDL_Init(SDL_INIT_EVERYTHING);

	int count = 0;
	while (count<500)
	{
		SDL_Event event;
		SDL_PollEvent(&event);
		if(event.type == SDL_QUIT){
			break;
		}
		lvp_sleep(10);
		count++;
	}
	lvp_core_free(core);
	getchar();
    return 0;
}
