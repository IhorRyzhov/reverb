/** 
 * @file main.c
 * @brief
 */
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "io-audio/io-audio.h"

static bool b_exit = false;

static void signalHandler(int arg)
{
    b_exit = true;
	printf("signalHandler()\n");
}

int main(void)
{
	signal(SIGINT, signalHandler);

    ioAudio_init();

    while (false == b_exit)
    {
        sleep(1);
    }    

    ioAudio_deinit();

	return 0;
}
