#include "tests.h"

void test_leds()
{
    // printf("Test LED 1\r\n");
    // led1_set_red();
    // Delay_Ms(500);
    // led1_set_green();
    // Delay_Ms(500);
    // led1_set_blue();
    // Delay_Ms(500);
    // led1_off();

    // printf("Test LED 2\r\n");
    // led2_set_red();
    // Delay_Ms(500);
    // led2_set_green();
    // Delay_Ms(500);
    // led2_set_blue();
    // Delay_Ms(500);
    // led2_off();

    // printf("Test LED 3\r\n");
    // led3_set_red();
    // Delay_Ms(500);
    // led3_set_green();
    // Delay_Ms(500);
    // led3_set_blue();
    // Delay_Ms(500);
    // led3_off();

    // printf("Test LEDs\r\n");
    // led1_set_red();
    // led2_set_red();
    // led3_set_red();
    // Delay_Ms(500);

    // led1_set_green();
    // led2_set_green();
    // led3_set_green();
    // Delay_Ms(500);

    // led1_set_blue();
    // led2_set_blue();
    // led3_set_blue();
    // Delay_Ms(500);

    // leds_off();
    // Delay_Ms(500);
    // leds_on();
    // Delay_Ms(500);
    // leds_off();
    // Delay_Ms(500);
    // leds_on();
    // Delay_Ms(500);
    // leds_off();
    // Delay_Ms(500);

    printf("Test rainbow effect\r\n");
    for (int i = 0; i < 100; i++)
    {
        static uint8_t counter = 1;
        APP_DBG("Counter: %d", counter);
        counter++;
        leds_set_rainbow(); // Change the colors every time its called
        Delay_Ms(100);
    }

    printf("Test finished\r\n");
    leds_off();
}