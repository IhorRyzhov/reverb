#ifndef GPIO_GPIO_H_
#define GPIO_GPIO_H_

#include <stdint.h>
#include <stdbool.h>

void gpio_init(uint8_t u8L_gpio, bool bL_direction);

void gpio_deinit(uint8_t u8L_gpio);

void gpio_write(uint8_t u8L_gpio, bool bL_value);

bool gpio_read(uint8_t u8L_gpio);

#endif /* GPIO_GPIO_H_ */
