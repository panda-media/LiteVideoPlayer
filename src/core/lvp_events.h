#ifndef _LVP_EVENTS_H_
#define _LVP_EVENTS_H_

#define LVP_EVENT_OPEN      "LVP_EVENT_OPEN"    ///< lvp open status, data NULL
#define LVP_EVENT_CLOSE     "LVP_EVENT_CLOSE"   ///< lvp close status, data NULL
#define LVP_EVENT_PLAYING   "LVP_EVENT_PLAYING" ///< lvp play status, data NULL
#define LVP_EVENT_BUFFING   "LVP_EVENT_BUFFING" ///< lvp buff status, data NULL
#define LVP_EVENT_ERROR     "LVP_EVENT_ERROR"   ///< lvp error status, data NULL

#define LVP_EVENT_PLAY      "LVP_EVENT_PLAY"    ///< play, data NULL
#define LVP_EVENT_STOP      "LVP_EVENT_STOP"    ///< stop play, data NULL
#define LVP_EVENT_SET_URL   "LVP_EVENT_SET_URL" ///< set play url, data const char* play path

#define LVP_EVENT_SEEK      "LVP_EVENT_SEEK"    ///< seek to time, data int64_t time for seek to
#define LVP_EVENT_PAUSE     "LVP_EVENT_PAUSE"   ///< pause play, data NULL
#define LVP_EVENT_RESUME    "LVP_EVENT_RESUME"  ///< resume play, data NULL
#define LVP_EVENT_PLAY_ON   "LVP_EVENT_PLAY_ON" ///< set media start time ,data int64_t time for microsecond

#define LVP_EVENT_CHANGE_AUDIO_FORMAT "LVP_EVENT_CHANGE_AUDIO_FORMAT"


#define LVP_EVENT_CHANGE_STREAM "LVP_EVENT_CHANGE_STREAM"///< user change stream
                                                        /// if media has multiple stream
                                                        ///most media has multiple audio stream and multiple sub


#define LVP_EVENT_CHANGE_TEMPO  "LVP_EVENT_CHANGE_TEMPO"    ///< change audio tempo, data double eg. 1.0 for narmal, 2.0 for 2 time speed

#define LVP_EVENT_OPEN_ERROR    "LVP_EVENT_OPEN_ERROR"
#define LVP_EVENT_OPEN_DONE     "LVP_EVENT_OPEN_DONE"
#define LVP_EVENT_SELECT_STREAM "LVP_EVENT_SELECT_STREAM"

#define LVP_EVENT_READER_EOF        "LVP_EVENT_READER_EOF"
#define LVP_EVENT_READER_ERROR      "LVP_EVENT_READER_ERROR"
#define LVP_EVENT_READER_GOT_FRAME  "LVP_EVENT_READER_GOT_FRAME"
#define LVP_EVENT_READER_SEND_FRAME "LVP_EVENT_READER_SEND_FRAME"

#define LVP_EVENT_FILTER_GOT_PKT    "LVP_EVENT_FILTER_GOT_PKT"
#define LVP_EVENT_FILTER_GOT_FRAME  "LVP_EVENT_FILTER_GOT_FRAME"
#define LVP_EVENT_FILTER_SEND_PKT   "LVP_EVENT_FILTER_SEND_PKT"
#define LVP_EVENT_FILTER_SEND_FRAME "LVP_EVENT_FILTER_SEND_FRAME"

#define LVP_EVENT_REQ_PKT   "LVP_EVENT_REQ_PKT"
#define LVP_EVENT_REQ_FRAME "LVP_EVENT_REQ_FRAME"
#define LVP_EVENT_REQ_SUB	"LVP_EVENT_REQ_SUB" //subtitle

#define LVP_EVENT_DECODER_GOT_FRAME     "LVP_EVENT_DECODER_GOT_FRAME"
#define LVP_EVENT_DECODER_SEND_FRAME    "LVP_EVENT_DECODER_SEND_FRAME"
#define LVP_EVENT_DECODER_SEND_SUB		"LVP_EVENT_DECODER_SEND_SUB"

#define LVP_EVENT_UPDATE_AUDIO  "LVP_EVENT_UPDATE_AUDIO"
#define LVP_EVENT_UPDATE_VIDEO  "LVP_EVENT_UPDATE_VIDEO"
#define LVP_EVENT_UPDATE_SUB	"LVP_EVENT_UPDATE_SUB"

#endif