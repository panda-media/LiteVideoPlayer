#ifndef _LVP_CPP_H_
#define _LVP_CPP_H_
int lvp_cpp_test();

void* lvp_create_soundtouch();

int lvp_soundtouch_set_tempo(void *st, int tempo);

int lvp_soundtouch_set_channels(void *st, int channels);

int lvp_soundtouch_set_rate(void *st, double rate);

#endif