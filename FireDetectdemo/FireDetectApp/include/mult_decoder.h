#ifndef __MULT_DECODER_H__
#define __MULT_DECODER_H__
void init_window();

int init_camera(char *name, int count);

void init_decoder();
void deinit_decoder();

void start_player_process();

void start_detect_process(char *cloud_dev_name);
#endif


