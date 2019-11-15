#include "../../core/lvp_core.h"
#include <SDL2/SDL.h>


int main(int argc, char **argv){

    LVPCore *core = lvp_core_alloc();
    lvp_core_set_option(core,"-readers abcreader");
	lvp_core_set_url(core, "/home/fgodt/test.flv");
    lvp_core_play(core);
	SDL_Init(SDL_INIT_EVERYTHING);

	while (1)
	{
		SDL_Event event;
		SDL_PollEvent(&event);
		if(event.type == SDL_QUIT){
			break;
		}
		lvp_sleep(10);
	}
    return 0;
}
