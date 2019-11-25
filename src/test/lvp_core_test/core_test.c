#include "../../core/lvp_core.h"
#include <SDL2/SDL.h>



int main(int argc, char** argv) {

	int count = 0;
	LVPCore* core = lvp_core_alloc();
	lvp_core_set_option(core, "-readers abcreader -lvphw dxva -lvpnhw 1 -lvpop_codec h264_d3d11va");
//	lvp_core_set_url(core, "C:\\Ñ¸À×ÏÂÔØ\\oupt\\oupt.mkv");
	lvp_core_set_url(core, "d:/bb.mkv");
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
