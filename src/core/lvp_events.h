#ifndef _LVP_EVENTS_H_
#define _LVP_EVENTS_H_

#define LVP_EVENT_OPEN      "LVP_EVENT_OPEN"
#define LVP_EVENT_CLOSE     "LVP_EVENT_CLOSE"

#define LVP_EVENT_PLAY      "LVP_EVENT_PLAY"
#define LVP_EVENT_STOP      "LVP_EVENT_STOP"
#define LVP_EVENT_SET_URL   "LVP_EVENT_SET_URL"

#define LVP_EVENT_SEEK      "LVP_EVENT_SEEK"
#define LVP_EVENT_PAUSE     "LVP_EVENT_PAUSE"
#define LVP_EVENT_RESUME    "LVP_EVENT_RESUME"

#define LVP_EVENT_CHANGE_STREAM "LVP_EVENT_CHANGE_STREAM"///< user change stream
                                                        /// if media has multiple stream
                                                        ///most media has multiple audio stream and multiple sub

#define LVP_EVENT_OPEN_ERROR    "LVP_EVENT_OPEN_ERROR"
#define LVP_EVENT_OPEN_DONE     "LVP_EVENT_OPEN_DONE"
#define LVP_EVENT_SELECT_STREAM "LVP_EVENT_SELECT_STREAM"

#define LVP_EVENT_READER_EOF    "LVP_EVENT_READER_EOF"
#define LVP_EVENT_READER_ERROR  "LVP_EVENT_READER_ERROR"
#define LVP_EVENT_READER_GOT_FRAME  "LVP_EVENT_READER_GOT_FRAME"

#endif