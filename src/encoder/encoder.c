#include <time.h>
#include <signal.h>
#include <pthread.h>
#include "../gpio/gpio.h"
#include "encoder.h"

#define MS_TO_NS(ms) ((ms) * 1000 * 1000)


encoderCallback_t p_callback = NULL;

// todo : deinit timer

static void* encoder_thread(void* vPL_data)
{
	static bool bL_rPrevState = false;
	static bool bL_lPrevState = false;

	bool bL_rState = false;
	bool bL_lState = false;

	while (1)
	{
		bL_rState = false;
		bL_lState = false;

		bL_rState = gpio_read(ENCODER_GPIO_R);
		bL_lState = gpio_read(ENCODER_GPIO_L);

		if (bL_rState != bL_lState)
		{
			if (!((bL_rPrevState != bL_rState) && (bL_lPrevState != bL_lState)))
			{
				if (bL_rPrevState != bL_rState) // turn right
				{
					p_callback(eEncoderCallbackNotificationTurnRight, NULL);
				}

				if (bL_lPrevState != bL_lState) // turn left
				{
					p_callback(eEncoderCallbackNotificationTurnLeft, NULL);
				}
			}
		}

		bL_rPrevState = bL_rState;
		bL_lPrevState = bL_lState;
	}

	return NULL;
}


void encoder_init(encoderCallback_t pL_callback)
{
	p_callback = pL_callback;

	gpio_init(ENCODER_GPIO_L, false);
	gpio_init(ENCODER_GPIO_R, false);

	pthread_t u64L_thread;

	pthread_attr_t objL_attr;

	pthread_attr_init(&objL_attr);

	pthread_attr_setdetachstate(&objL_attr, PTHREAD_CREATE_DETACHED);

	pthread_create(&u64L_thread, NULL, encoder_thread, NULL);

	pthread_detach(u64L_thread);

}

void encoder_deinit(void)
{
	//gpio_deinit();
}

