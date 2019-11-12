#include "lvp_hwaccel.h"
#include "../core/lvp_map.h"

LVPHWAccel *lvp_hwaccel_get_hwaccel(LVPMap *options){
    LVPHWAccel *hw = (LVPHWAccel*)lvp_mem_mallocz(sizeof(*hw));
    const char *hwaccel = lvp_map_get(options,"hwaccel");
    if(!strcmp(hwaccel,"none")){
        hw->id = LVP_HWACCEL_NONE;
    }
    else if(!strcmp(hwaccel,"auto")){
        hw->id = LVP_HWACCEL_AUTO;
    }else
    {
        enum AVHWDeviceType type;
        int i;
        for (size_t i = 0; i < hwaccels[i].name; i++)
        {
            if(!strcmp(hwaccels[i].name,hwaccel)){
                hw->id = hwaccels[i].id;
                break;
            }
        }

        if(!hw->id){
            type = av_hwdevice_find_type_by_name(hwaccel);
            if(!type != AV_HWDEVICE_TYPE_NONE){
                hw->id = LVP_HWACCEL_GENERIC;
                hw->type = type;
            }
        }

        if(!hw->id){
            lvp_waring(NULL,"lvp get hwaccel error for %s",hwaccel);
            lvp_mem_free(hw);
            return NULL;
        }

        hw->pix_fmt = AV_PIX_FMT_NONE;
        
    }

    return hw;
    
}