#include "lvp_event.h"


LVPEvent *lvp_event_alloc(void *data, const char *name,LVP_BOOL must_handle){
    LVPEvent *ev = (LVPEvent*)lvp_mem_mallocz(sizeof(*ev));
    ev->data = data;
    ev->must_handle = must_handle;
    lvp_str_dump(name,&ev->event_name);
    return ev;
}

void lvp_event_free(LVPEvent *ev){
    if(ev->event_name){
        lvp_mem_free(ev->event_name);
    }
    lvp_mem_free(ev);
}

///
// listener
///

LVPEventListener *lvp_event_listener_alloc(){
    LVPEventListener *l = (LVPEventListener*)lvp_mem_mallocz(sizeof(*l));
    return l;
}

void lvp_event_listener_free(LVPEventListener *l){
    free(l);
}

///
// handler
///
LVPEventHandler *lvp_event_handler_alloc(const char *name){
    LVPEventHandler *h = (LVPEventHandler*)lvp_mem_mallocz(sizeof(*h));
    lvp_str_dump(name,&h->event_name);
    h->listeners = lvp_list_alloc();
    return h;
}
void lvp_event_handler_free(LVPEventHandler *h){
    if(h->event_name){
        lvp_mem_free(h->event_name);
    }
    if(h->listeners){
        lvp_list_free(h->listeners);
    }
    free(h);
}

static void lvp_event_handler_custom_free(void *h, void *usr_data){
    lvp_event_handler_free((LVPEventHandler*)h);
}

static void lvp_event_listener_custom_free(void *l, void *usr_data){
    lvp_event_listener_free((LVPEventListener*)l);
}

///
// control
///

LVPEventControl *lvp_event_control_alloc(){
    LVPEventControl *ctl = (LVPEventControl*)lvp_mem_mallocz(sizeof(*ctl));
    ctl->handlers = lvp_map_alloc();
    return ctl;
}

void lvp_event_control_free(LVPEventControl *ctl){
    if (ctl->handlers)
    {
        lvp_map_free(ctl->handlers);
    }
    lvp_mem_free(ctl);
}

int lvp_event_control_add_listener(LVPEventControl *ctl,
        const char *key, lvp_event_call call, void *usr_data){

    assert(ctl);
    assert(key);
    assert(call);
    //create listener
    LVPEventListener *l = lvp_event_listener_alloc();
    l->call = call;
    l->usr_data = usr_data;

    //find handler
    LVPEventHandler *h = lvp_map_get(ctl->handlers,key);
    //if not find create 
    if(!h){
        h = lvp_event_handler_alloc(key);
        lvp_map_add(ctl->handlers,key,h,NULL,lvp_event_handler_custom_free,LVP_TRUE);
    }
    lvp_list_add(h->listeners,l,NULL,lvp_event_listener_custom_free,LVP_TRUE);

    return LVP_OK;
}

int lvp_event_control_send_event(LVPEventControl *ctl, LVPEvent *ev){
    assert(ctl);
    assert(ev);
    LVPEventHandler *h = lvp_map_get(ctl->handlers,ev->event_name);
    //no handler find
    if(!h){
        if(ev->must_handle == LVP_TRUE){
            lvp_waring(NULL,"no handler for %s",ev->event_name);
        }
        return LVP_E_NO_MEDIA;
    }
    LVPListEntry *listener_entry = h->listeners->entrys;
    int ret = 0;
    while (listener_entry)
    {
        LVPEventListener *l = (LVPEventListener*)listener_entry->data;
        int status = l->call(ev,l->usr_data);
        if(status != LVP_OK && strcmp(ev->event_name,LVP_EVENT_READER_SEND_FRAME) ){
            lvp_waring(NULL,"event %s listener return status %d",ev->event_name,status);
        }
        ret |= status;
        listener_entry = listener_entry->next;
    }
    return ret;
    
}