/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr.h>
#include <soc.h>
#include <device.h>
#include <drivers/gpio.h>
#include <sys/util.h>
#include <logging/log.h>
#include <nrfx.h>
#include "gus_leds.h"

LOG_MODULE_REGISTER(gus_leds, CONFIG_DK_LIBRARY_LOG_LEVEL);


struct gpio_pin {
	const char * const port;
	const uint8_t number;
};


static const struct gpio_pin led_pins[] = {
#if DT_NODE_EXISTS(DT_ALIAS(led0))
	{DT_GPIO_LABEL(DT_ALIAS(led0), gpios),
	 DT_GPIO_PIN(DT_ALIAS(led0), gpios)},
#endif
#if DT_NODE_EXISTS(DT_ALIAS(led1))
	{DT_GPIO_LABEL(DT_ALIAS(led1), gpios),
	 DT_GPIO_PIN(DT_ALIAS(led1), gpios)},
#endif
#if DT_NODE_EXISTS(DT_ALIAS(led2))
	{DT_GPIO_LABEL(DT_ALIAS(led2), gpios),
	 DT_GPIO_PIN(DT_ALIAS(led2), gpios)},
#endif
#if DT_NODE_EXISTS(DT_ALIAS(led3))
	{DT_GPIO_LABEL(DT_ALIAS(led3), gpios),
	 DT_GPIO_PIN(DT_ALIAS(led3), gpios)},
#endif
#if DT_NODE_EXISTS(DT_ALIAS(led4))
	{DT_GPIO_LABEL(DT_ALIAS(led4), gpios),
	 DT_GPIO_PIN(DT_ALIAS(led4), gpios)},
#endif
#if DT_NODE_EXISTS(DT_ALIAS(led5))
	{DT_GPIO_LABEL(DT_ALIAS(led5), gpios),
	 DT_GPIO_PIN(DT_ALIAS(led5), gpios)},
#endif
};

enum state {
	STATE_WAITING,
	STATE_SCANNING,
};


static const struct device *led_devs[ARRAY_SIZE(led_pins)];





int gus_leds_init(void)
{
	int err;

	for (size_t i = 0; i < ARRAY_SIZE(led_pins); i++) {
		led_devs[i] = device_get_binding(led_pins[i].port);
		if (!led_devs[i]) {
			LOG_ERR("Cannot bind gpio device");
			return -ENODEV;
		}

		err = gpio_pin_configure(led_devs[i], led_pins[i].number,
					 GPIO_OUTPUT);
		if (err) {
			LOG_ERR("Cannot configure LED gpio");
			return err;
		}
	}

	return gus_set_leds_state(DK_NO_LEDS_MSK, DK_ALL_LEDS_MSK);
}

int gus_set_leds(uint32_t leds)
{
	return gus_set_leds_state(leds, DK_ALL_LEDS_MSK);
}

int gus_set_leds_state(uint32_t leds_on_mask, uint32_t leds_off_mask)
{
	if ((leds_on_mask & ~DK_ALL_LEDS_MSK) != 0 ||
	   (leds_off_mask & ~DK_ALL_LEDS_MSK) != 0) {
		return -EINVAL;
	}

	for (size_t i = 0; i < ARRAY_SIZE(led_pins); i++) {
		if ((BIT(i) & leds_on_mask) || (BIT(i) & leds_off_mask)) {
			uint32_t val = (BIT(i) & leds_on_mask) ? (1) : (0);

			if (IS_ENABLED(CONFIG_DK_LIBRARY_INVERT_LEDS)) {
				val = 1 - val;
			}

			int err = gpio_pin_set_raw(led_devs[i],
						   led_pins[i].number, val);
			if (err) {
				LOG_ERR("Cannot write LED gpio");
				return err;
			}
		}
	}

	return 0;
}

int gus_set_led(uint8_t led_idx, uint32_t val)
{
	int err;

	if (led_idx > ARRAY_SIZE(led_pins)) {
		LOG_ERR("LED index out of the range");
		return -EINVAL;
	}
	err = gpio_pin_set_raw(led_devs[led_idx], led_pins[led_idx].number,
		IS_ENABLED(CONFIG_DK_LIBRARY_INVERT_LEDS) ? !val : val);
	if (err) {
		LOG_ERR("Cannot write LED gpio");
	}
	return err;
}

int gus_set_led_on(uint8_t led_idx)
{
	return gus_set_led(led_idx, 1);
}

int gus_set_led_off(uint8_t led_idx)
{
	return gus_set_led(led_idx, 0);
}
