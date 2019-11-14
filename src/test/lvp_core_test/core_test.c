#include "../../core/lvp_core.h"
#include <SDL2/SDL.h>

void* test1(void* data) {
	UINT64 j = 0;
	while (1)
	{
		j++;
	}
}

void* test2(void* data) {
	UINT64 j = 0;
	while (1)
	{
		j++;
	}
}

void* test3(void* data) {
	UINT64 j = 0;
	while (1)
	{
		j++;
	}
}

int main(int argc, char **argv){

    LVPCore *core = lvp_core_alloc();
    lvp_core_set_option(core,"-readers abcreader");
	lvp_core_set_url(core, "d:/test.flv");
    lvp_core_play(core);
	SDL_Init(SDL_INIT_EVERYTHING);

	while (1)
	{
		SDL_Event event;
		SDL_PollEvent(&event);
		lvp_sleep(10);
	}
    getchar();
    return 0;
}