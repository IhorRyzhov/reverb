#ifndef ENCODER_ENCODER_H_
#define ENCODER_ENCODER_H_

#include <stdbool.h>

#define ENCODER_GPIO_L 11
#define ENCODER_GPIO_R 12
//#define ENCODER_GPIO_B 6 // UNUSED

#define ENCODER_POLLING_PERIOD_MS 5

enum EncoderCallbackNotification
{
	eEncoderCallbackNotificationTurnRight,
	eEncoderCallbackNotificationTurnLeft,
};
typedef enum EncoderCallbackNotification encoderCallbackNotification_t;

typedef void (*encoderCallback_t)(encoderCallbackNotification_t eL_notification, void* vPL_data);


void encoder_init(encoderCallback_t pL_callback);

void encoder_deinit(void);

#endif /* ENCODER_ENCODER_H_ */
