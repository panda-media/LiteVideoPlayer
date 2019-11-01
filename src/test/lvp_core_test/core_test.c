#include "../../core/lvp_core.h"

int main(){
    LVPCore *core = lvp_core_alloc();
    lvp_core_set_option(core,"-reader abcreader");
    lvp_core_set_url(core,"/home/fgodt/jtest.mp4");
    lvp_core_play(core);
    getchar();
    return 0;
}