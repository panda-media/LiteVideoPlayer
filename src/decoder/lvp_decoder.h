#ifndef _LVP_DECODER_H_
#define _LVP_DECODER_H_

#include "../core/lvp_core.h"
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

typedef struct lvp_decoder{
    LVPEventControl *ctl;
    LVPLog *log;
    LVPMap *options;

    AVCodec *codec;
    AVCodecContext *avctx;
    AVFrame *iframe;
	AVFrame* sw_frame;
    AVPacket *ipkt;
    lvp_thread dec_thread;

    int decoder_thread_run;

    enum AVMediaType codec_type;
    AVStream *stream;

	lvp_mutex mutex;

	//hwaccel
	AVBufferRef* hw_device_ctx;
	enum AVPixelFormat hw_pix_fmt;
	enum AVHWDeviceType hw_dev_type;
}LVPDecoder;

#endif