#include "lvp_video_render.h"
#include <libavcodec/avcodec.h>

int handle_update(LVPEvent *ev,void *usrdata){

    //todo test
    return LVP_OK;
    //test code 

    LVPVideoRender *r = (LVPVideoRender*)usrdata;

    AVFrame *f = (AVFrame*)ev->data;
	if (f->width <= 0) {
		return LVP_OK;
	}

	lvp_mutex_lock(&r->mutex);
    if (r->texture == NULL)
    {

			r->texture = SDL_CreateTexture(r->renderer, SDL_PIXELFORMAT_IYUV,
				SDL_TEXTUREACCESS_STREAMING, f->width, f->height);
    }

	

	if (f->format == AV_PIX_FMT_YUV420P) {

		SDL_UpdateYUVTexture(r->texture, NULL,
			f->data[0], f->linesize[0],
			f->data[1], f->linesize[1],
			f->data[2], f->linesize[2]);
	}
	else
	{

    //for test

		SDL_UpdateYUVTexture(r->texture, NULL,
			f->data[0], f->linesize[0],
			f->data[1], f->linesize[1]/2,
			f->data[1], f->linesize[1]/2);
	}
    
    SDL_RenderClear(r->renderer);
    SDL_RenderCopy(r->renderer,r->texture,NULL,&r->rect);
    SDL_RenderPresent(r->renderer);
	lvp_mutex_unlock(&r->mutex);
    return LVP_OK;

}



static int module_init(struct lvp_module *module, 
                                    LVPMap *options,
                                    LVPEventControl *ctl,
                                    LVPLog *log){
    assert(module);
    assert(ctl);
    assert(log);

    //todo test
    lvp_event_control_add_listener(ctl,LVP_EVENT_UPDATE_VIDEO,handle_update,NULL);
    return LVP_OK;
    //test code 


    LVPVideoRender *r = (LVPVideoRender*)module->private_data;
    r->ctl = ctl;
    r->log = lvp_log_alloc(module->name);
    r->log->log_call = log->log_call;
    r->log->usr_data = log->usr_data;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);


    r->rect.w = 1280;
    r->rect.h = 720;

    r->window = SDL_CreateWindow("Lite Video Player",SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,r->rect.w,r->rect.h,SDL_WINDOW_SHOWN);

    r->renderer = SDL_CreateRenderer(r->window,-1,0);


	lvp_mutex_create(&r->mutex);

    int ret = lvp_event_control_add_listener(ctl,LVP_EVENT_UPDATE_VIDEO,handle_update,r);
    if(ret!=LVP_OK){
        lvp_error(r->log,"add listener error",NULL);
        return LVP_E_FATAL_ERROR;
    }


    return LVP_OK;

}


static void module_close(struct lvp_module *module){
    assert(module);

	LVPVideoRender* r = (LVPVideoRender*)module->private_data;
	if (r->log) {
		lvp_log_free(r->log);
	}
	if (r->texture) {
		SDL_DestroyTexture(r->texture);
	}
	if (r->renderer) {
		SDL_DestroyRenderer(r->renderer);
	}
	if (r->window) {
		SDL_DestroyWindow(r->window);
	}
	lvp_mem_free(r);
	module->private_data = NULL;

}

LVPModule lvp_video_render = {
    .version = lvp_version,
    .name = "LVP_VIDEO_RENDER",
    .type = LVP_MODULE_CORE|LVP_MODULE_RENDER,
    .private_data_size = sizeof(LVPVideoRender),
    .private_data = NULL,
    .module_init = module_init,
    .module_close = module_close,
};