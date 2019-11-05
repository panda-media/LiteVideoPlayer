#include "lvp_pkt_filter.h"

static int handle_reader_send_frame(LVPEvent *ev, void *usr_data){
    assert(ev);
    LVPPktFilter *f = (LVPPktFilter*)usr_data;

    //for sub module use
    LVPSENDEVENT(f->ctl,LVP_EVENT_FILTER_GOT_PKT,ev->data);

    //for other core module use
    LVPEvent *must_handle_ev = lvp_event_alloc(ev->data,LVP_EVENT_FILTER_SEND_PKT,LVP_TRUE);
    int ret = lvp_event_control_send_event(f->ctl,must_handle_ev);
    
    return ret;
}

static int filter_init(struct lvp_module *module, 
                                    LVPMap *options,
                                    LVPEventControl *ctl,
                                    LVPLog *log){
    assert(module);
    assert(module->private_data);
    assert(ctl);
    LVPPktFilter *filter = (LVPPktFilter*)module->private_data;
    filter->log = lvp_log_alloc(module->name);
    filter->log->log_call = log->log_call;
    filter->log->usr_data = log->usr_data;

    filter->ctl = ctl;

    int ret = lvp_event_control_add_listener(filter->ctl,LVP_EVENT_READER_SEND_FRAME,handle_reader_send_frame,filter);
    if(ret != LVP_OK){
        lvp_error(filter->log,"add listener for %s error return %d",LVP_EVENT_READER_SEND_FRAME,ret);
        return ret;
    }

    //init sub module
    filter->modules = lvp_list_alloc();
    void *op = NULL;
    for (LVPModule *m = NULL; (m = lvp_module_iterate(&op)) != NULL; )
    {
        if(m->type == LVP_MODULE_PKT_FILTER){
            LVPModule *c = lvp_module_create_module(m);
            if(c!=NULL){
                int ret = lvp_module_init(c,options,ctl,log);
                if(ret != LVP_OK){
                    lvp_module_close(c);
                    lvp_mem_free(c);
                }
                lvp_list_add(filter->modules,c,NULL,NULL,1);
            }
        }
    }
    


    return LVP_OK;
}

static void filter_close(struct lvp_module *module){
    assert(module);
}


LVPModule lvp_pkt_filter = {
    .version = lvp_version,
    .name = "LVP_PKT_FILTER",
    .type = LVP_MODULE_CORE|LVP_MODULE_PKT_FILTER,
    .private_data_size = sizeof(LVPPktFilter),
    .private_data = NULL,
    .module_init = filter_init,
    .module_close = filter_close,
};