#include "../../core/lvp_core.h"
#include <SDL2/SDL.h>


int main(int argc, char **argv){

	int count = 0;
	while (count<100)
	{
	
    LVPCore *core = lvp_core_alloc();
    lvp_core_set_option(core,"-readers abcreader -lvphw qsv");
	lvp_core_set_url(core, "/home/fgodt/test.flv");
    lvp_core_play(core);
	lvp_sleep(50);
	lvp_core_free(core);
	count++;
	}
    return 0;
}
