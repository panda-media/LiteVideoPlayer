#ifndef _LVP_READER_MODULE_H_
#define _LVP_READER_MODULE_H_

#include "../core/lvp_core.h"
#include <libavformat/avformat.h>

typedef enum lvp_reader_status{
    LVP_READER_OPEN,
    LVP_READER_CLOSE,
    LVP_READER_RUNNING,
    LVP_READER_ERROR,
}LVPReaderStatus;

typedef struct lvp_reader_module{

    LVPEventControl *ctl; ///< main event control
    LVPLog          *log; ///< log 

    AVFormatContext *avctx; ///< input context

    AVStream        *astream; ///< select audio stream
    AVStream        *vstream; ///< select video stream
    AVStream        *sub_stream; ///< select sub stream

    LVP_BOOL        is_reader_thread_run;
    lvp_thread      reader_thread;
    lvp_mutex       avctx_mutex;

    char            *input_url;
    LVP_BOOL        is_interrupt;

    LVPModule       **reader_module;

    LVPReaderStatus status;

}LVPReaderModule;



#endif