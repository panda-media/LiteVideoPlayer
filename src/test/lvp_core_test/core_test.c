#include "../../core/lvp_core.h"

int main(){
    LVPCore *core = lvp_core_alloc();
    lvp_core_set_url(core,"http://192.168.0.107/test.mp4");
    lvp_core_play(core);
    getchar();
    return 0;
}