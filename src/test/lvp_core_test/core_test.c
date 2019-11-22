#include "../../core/lvp_core.h"
#include <SDL2/SDL.h>



int main(int argc, char** argv) {

	int count = 0;
	LVPCore* core = lvp_core_alloc();
	lvp_core_set_option(core, "-readers abcreader -lvphw cuda");
	lvp_core_set_url(core, "https://live-jd2-hdl.huomaotv.cn/live/dkwyIW30362_8000p.flv?t=1574389806&r=98333181706&stream=dkwyIW30362&rid=oubvc2y3v&token=c7fe8fe3641276f6187a26e0a6dea53b&url=https%3A%2F%2Flive-jd2-hdl.huomaotv.cn%2Flive%2FdkwyIW30362_8000p.flv&from=huomaoh5room");
	lvp_core_play(core);
	SDL_Event ev;
	while (1)
	{
		SDL_PollEvent(&ev);
		if (ev.type == SDL_QUIT)
		{
			break;
		}
		SDL_Delay(30);
	}
	lvp_core_free(core);
	count++;
	return 0;
}
