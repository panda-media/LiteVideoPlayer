#include "../core/lvp_core.h"
#include <libswresample/swresample.h>

typedef struct lvp_audio_format {
	int channels;
	int format;
	int sample_rate;
}LVPAudioFormat;

typedef struct lvp_audio_resample {
	struct SwrContext* swr;
	LVPEventControl* ctl;
	LVPLog* log;
	LVPAudioFormat target;
	LVPAudioFormat src;
}LVPAudioResample;

static int handle_frame(LVPEvent *ev,void *usrdata){
	AVFrame *frame = (AVFrame*)ev->data;

	//video frame
	if (frame->width > 0) {
		return LVP_OK;
	}

	LVPAudioResample *r = (LVPAudioResample*)usrdata;
	if(r->target.format == AV_SAMPLE_FMT_NONE && frame->format > AV_SAMPLE_FMT_DBL){
		//set planner to signal planner
		r->target.format = frame->format-5;
		//r->target.format = AV_SAMPLE_FMT_S16;
		r->target.channels = frame->channels;
		r->target.sample_rate = frame->sample_rate;
	}

    if(r->target.format == AV_SAMPLE_FMT_NONE){
		return LVP_OK;
	}

	if(frame->format != r->src.format ||
		frame->channels != r->src.channels ||
		frame->sample_rate != r->src.sample_rate){

		r->src.format = frame->format;
		r->src.channels = frame->channels;
		r->src.sample_rate = frame->sample_rate;

		if(r->swr){
			swr_close(r->swr);
			swr_free(&r->swr);
		}

		r->swr = swr_alloc_set_opts(NULL,av_get_default_channel_layout(r->target.channels),
		r->target.format,r->target.sample_rate,frame->channel_layout,frame->format,frame->sample_rate,
		0,0);
		if(r->swr == NULL){
			lvp_error(r->log,"audio resample error",NULL);
			return LVP_E_FATAL_ERROR;
		}
		int ret = swr_init(r->swr);
		if(ret<0){
			lvp_error(r->log,"swr init error",NULL);
			return LVP_E_FATAL_ERROR;
		}
	}

	AVFrame *dstframe = av_frame_alloc();
	dstframe->format = r->target.format;
	dstframe->channels = r->target.channels;
	dstframe->sample_rate = r->target.sample_rate;
	dstframe->nb_samples = frame->nb_samples * r->target.sample_rate/frame->sample_rate+256;

	int ret = av_frame_get_buffer(dstframe,0);
	if(ret<0){
		lvp_error(r->log,"get buffer error",NULL);
		av_frame_free(&dstframe);
		return LVP_E_FATAL_ERROR;
	}
	ret = swr_convert(r->swr,dstframe->data,dstframe->nb_samples,(const uint8_t **)frame->data,frame->nb_samples);
	if(ret<=0){
		lvp_error(r->log,"swr convert error",NULL);
		av_frame_free(&dstframe);
		return LVP_E_FATAL_ERROR;
	}
	av_frame_copy_props(dstframe, frame);
	dstframe->nb_samples = ret;
	ev->data = dstframe;
	//av_frame_free(&frame);

	return LVP_OK;

}


static int filter_init(struct lvp_module* module,
	LVPMap* options,
	LVPEventControl* ctl,
	LVPLog* log) {
	assert(module);
	assert(ctl);
	assert(log);

	LVPAudioResample* r = (LVPAudioResample*)module->private_data;
	r->log = lvp_log_alloc(module->name);
	r->log->log_call = log->log_call;
	r->log->usr_data = log->usr_data;

	r->target.format = AV_SAMPLE_FMT_NONE;
	r->src.format = AV_SAMPLE_FMT_NONE;
	r->ctl = ctl;

	int ret = lvp_event_control_add_listener(r->ctl,LVP_EVENT_FILTER_GOT_FRAME,handle_frame,r);
	if(ret != LVP_OK){
		lvp_error(r->log,"add %s listener error",LVP_EVENT_FILTER_GOT_FRAME);
		return LVP_E_FATAL_ERROR;
	}

	//todo use options set target format

	return LVP_OK;
}

static void filter_close(struct lvp_module* module){
	assert(module);
}

LVPModule lvp_audio_resample= {
	.version = lvp_version,
	.name = "LVP_AUDIO_RESAMPLE",
	.type = LVP_MODULE_FRAME_FILTER,
	.private_data_size = sizeof(LVPAudioResample),
	.private_data = NULL,
	.module_init = filter_init,
	.module_close = filter_close,
};