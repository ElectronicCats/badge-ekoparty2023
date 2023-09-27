#ifndef LEDS_H
#define LEDS_H

#include <stdio.h>
#include <string.h>

#include "ws2812b_dma_spi_led_driver.h"
#include "color_utilities.h"

void leds_init();
uint32_t WS2812BLEDCallback(int ledno); // Callback that you must implement.
void leds_on();
void leds_off();

void led1_on();
void led1_off();
void led1_set_red();
void led1_set_green();
void led1_set_blue();

void led2_on();
void led2_off();

void led3_on();
void led3_off();

#endif