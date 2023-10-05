#include "leds.h"
#include "debug.h"

#define LED_OFF 0
#define LED1_RED 0x00007F
#define LED1_GREEN 0x7F007F00
#define LED1_BLUE 0x007F0000
#define LED2_RED 0x010002E6
#define LED2_GREEN 0x00002F00
#define LED2_BLUE 0x007F0000
#define LED3_RED 0x007F005D
#define LED3_GREEN 0x00007F00
#define LED3_BLUE 0x000015E2
#define DUMMY_COLOR 0x00007F00

int leds_number = 0;
int turn_off_leds;
int rainbow_mode;
uint32_t led1_color, led1_color_backup;
uint32_t led2_color, led2_color_backup;
uint32_t led3_color, led3_color_backup;

uint32_t random_color(int led_number);

void leds_init()
{
    leds_number = 3;
    turn_off_leds = 0;
    rainbow_mode = 0;
    led1_color = LED_OFF;
    led2_color = LED_OFF;
    led3_color = LED_OFF;

    WS2812BDMAInit();
    Delay_Ms(10); // Give some time to turn leds on
}

uint32_t WS2812BLEDCallback(int led_number)
{
    if (turn_off_leds)
    {
        return 0;
    }

    if (rainbow_mode)
    {
        return random_color(led_number);
    }

    if (led_number == 0)
    {
        return led1_color;
    }
    else if (led_number == 1)
    {
        return led2_color;
    }
    else if (led_number == 2)
    {
        return led3_color;
    }
    else
    {
        return DUMMY_COLOR;
    }
}

void leds_on()
{
    turn_off_leds = 0;
    WS2812BDMAStart(leds_number);
}

void leds_off()
{
    turn_off_leds = 1;
    WS2812BDMAStart(leds_number);
}

uint32_t random_color(int led_number)
{
    uint16_t phases[leds_number];

    for (int k = 0; k < leds_number; k++)
    {
        phases[k] += ((((rands[k & 0xff]) + 0xf) << 2) + (((rands[k & 0xff]) + 0xf) << 1)) >> 1;
    }

    uint8_t tween = 0;
    uint8_t index = (phases[led_number]) >> 8;
    uint8_t rsbase = sintable[index];
    uint8_t rs = rsbase >> 3;
    uint32_t fire = ((huetable[(rs + 190) & 0xff] >> 1) << 16) | (huetable[(rs + 30) & 0xff]) | ((huetable[(rs + 0)] >> 1) << 8);
    uint32_t ice = 0x7f0000 | ((rsbase >> 1) << 8) | ((rsbase >> 1));

    // Because this chip doesn't natively support multiplies, we are going to avoid tweening of 1..254.
    return TweenHexColors(fire, ice, ((tween + led_number) > 0) ? 255 : 0); // Where "tween" is a value from 0 ... 255
}

void leds_set_rainbow()
{
    rainbow_mode = 1;
    leds_number = 3;
    leds_on();
}

void led1_on()
{
    led1_color = led1_color_backup;
    leds_on();
}

void led1_off()
{
    led1_color_backup = led1_color;
    led1_color = LED_OFF;
    leds_on();
}

void led1_set_red()
{
    led1_color = LED1_RED;
    led1_color_backup = led1_color;
    leds_on();
}

void led1_set_green()
{
    led1_color = LED1_GREEN;
    led1_color_backup = led1_color;
    leds_on();
}

void led1_set_blue()
{
    led1_color = LED1_BLUE;
    led1_color_backup = led1_color;
    leds_on();
}

void led2_on()
{
    led2_color = led2_color_backup;
    leds_on();
}

void led2_off()
{
    led2_color_backup = led2_color;
    led2_color = LED_OFF;
    leds_on();
}

void led2_set_red()
{
    led2_color = LED2_RED;
    led2_color_backup = led2_color;
    leds_on();
}

void led2_set_green()
{
    led2_color = LED2_GREEN;
    led2_color_backup = led2_color;
    leds_on();
}

void led2_set_blue()
{
    led2_color = LED2_BLUE;
    led2_color_backup = led2_color;
    leds_on();
}

void led3_on()
{
    led3_color = led3_color_backup;

    if (led3_color == LED3_RED)
    {
        led3_set_red();
    }
    else if (led3_color == LED3_GREEN)
    {
        led3_set_green();
    }
    else if (led3_color == LED3_BLUE)
    {
        led3_set_blue();
    }
    else
    {
        leds_on();
    }
}

void led3_off()
{
    led3_color_backup = led3_color;
    led3_color = LED_OFF;
    leds_on();
}

void led3_set_red()
{
    leds_number = 3;
    led3_color = LED3_RED;
    leds_on();
}

void led3_set_green()
{
    leds_number = 3;
    led3_color = LED3_GREEN;
    leds_on();
}

void led3_set_blue()
{
    leds_number = 4;
    led3_color = LED3_BLUE;
    leds_on();
}
