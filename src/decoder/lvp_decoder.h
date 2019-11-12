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
    AVPacket *ipkt;
    lvp_thread dec_thread;

    int decoder_thread_run;

    enum AVMediaType codec_type;
    AVStream *stream;

}LVPDecoder;

#endif