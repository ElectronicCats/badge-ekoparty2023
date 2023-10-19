#ifndef LEDS_H
#define LEDS_H

#include "ws2812b_dma_spi_led_driver.h"
#include "color_utilities.h"
#include "HAL.h"
#include "app_mesh_config.h"

#define NO_DELAY 0
#define RAINBOW_DELAY_MS 100

#define LEDS_RAINBOW_EVENT 0x0001

#define true 1
#define false 0
#define TURN_OFF_DELAY_MS 5

extern tmosTaskID ledsTaskID;

void leds_init();
tmosEvents Leds_ProcessEvent(tmosTaskID taks_id, tmosEvents events);
uint32_t WS2812BLEDCallback(int led_number); // Callback that you must implement.
void leds_on();
void leds_off();
void leds_set_rainbow();

void led1_on();
void led1_off();
void led1_set_red();
void led1_set_green();
void led1_set_blue();

void led2_on();
void led2_off();
void led2_set_red();
void led2_set_green();
void led2_set_blue();

void led3_on();
void led3_off();
void led3_set_red();
void led3_set_green();
void led3_set_blue();

#endif