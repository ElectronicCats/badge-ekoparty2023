#include "leds.h"
#include "debug.h"

int leds_number = 4;

void leds_init()
{
    WS2812BDMAInit();
    Delay_Ms(10); // Give some time to turn leds on
}

uint32_t WS2812BLEDCallback(int ledno)
{
    if (ledno == 0)
    {
        // return 0x00007F; // red
        return 0x7F007F00; // green
        // return 0x007F0000; // blue
        return 0; // off
    }
    else if (ledno == 1)
    {
        // return 0x010002E6; // red
        // return 0x00002F00; // green
        return 0x007F0000; // blue
        return 0; // off
    }
    else if (ledno == 2)
    {
        // return 0x007F005D; // red
        // return 0x00007F00; // green
        return 0x000015E2; // blue
        // return 0; // off
    }
    else
    {
        return 0x00007F00;
    }
}

void leds_on()
{
    WS2812BDMAStart(leds_number);
}
