#include <string.h>
#include <math.h>
#include <glib.h>
#include <stdlib.h>
#include <thread>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <chrono>
#include "mult_decoder.h"
#include "res.h"

#define CAM_CONFIG_MAX 4 
#define INI_FILE_PATH "../camera_conf.ini"

int main(int argc, char *argv[])
{
	/*step 1 init window for display*/
	init_window ();
	/*step 2 init viedeo resource*/
	init_camera(INI_FILE_PATH, CAM_CONFIG_MAX);
	/*step 3 init decoder resource*/
	init_decoder();
	/*step 4 start fire/smoke detect process*/
	start_detect_process(CLOUDE_DEV_NAME);
    while(true)
		sleep(3);
	/*step 5 deinit decoder resource*/
	deinit_decoder();

	return 0;
}

