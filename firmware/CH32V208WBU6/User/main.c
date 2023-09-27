/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 USART Print debugging routine:
 USART1_Tx(PA9).
 This example demonstrates using USART1(PA9) as a print debug port output.

*/

#include "debug.h"

/* Global typedef */

/* Global define */

/* Global Variable */

#include <stdio.h>
#include <string.h>

#define WS2812DMA_IMPLEMENTATION
// #define WSRBG //For WS2816C's.
#define WSGRB // For SK6805-EC15
#define NR_LEDS 4

#include "ws2812b_dma_spi_led_driver.h"

#include "color_utilities.h"

uint16_t phases[NR_LEDS];
int frameno;
volatile int tween = -NR_LEDS;

// Callbacks that you must implement.
uint32_t WS2812BLEDCallback(int ledno)
{
    uint8_t index = (phases[ledno]) >> 8;
    uint8_t rsbase = sintable[index];
    // rsbase = 0x80;
    uint8_t rs = rsbase >> 3;
    uint32_t fire = ((huetable[(rs + 190) & 0xff] >> 1) << 16) | (huetable[(rs + 30) & 0xff]) | ((huetable[(rs + 0)] >> 1) << 8);
    uint32_t ice = 0x7f0000 | ((rsbase >> 1) << 8) | ((rsbase >> 1));

    // Because this chip doesn't natively support multiplies, we are going to avoid tweening of 1..254.
    
    // return TweenHexColors(fire, ice, ((tween + ledno) > 0) ? 255 : 0); // Where "tween" is a value from 0 ... 255
    // return ice;

    static uint32_t color = 0x00801811;
    color++;

    if (ledno == 0)
    {
        // return 0x00001BEA;
        // return 0x00007F; // red
        // return 0x7F007F00; // green
        return 0x007F0000; // blue
        // return 0xFFFFFF; // white more bright
        // return 0x007F0808; // white to test
        // return 0x7F0000; // purple
    }
    else if (ledno == 1)
    {
        // return 0x000039FF;
        // return color;
        // return 0x010002D1;
        // return 0x0100046F;
        // return 0x010002E6; // red
        // return 0x00002F00; // green
        return 0x007F0000; // blue
        // return 0xFF002F00; // green
        // return 0x00FFFFFF; // white more bright
        // return 0x007F7676; // white to test
        // return 0x00FF00FF; // purple
    }
    else if (ledno == 2)
    {
        // return color;
        // return TweenHexColors(fire, ice, ((tween + ledno) > 0) ? 255 : 0); // Where "tween" is a value from 0 ... 255
        // return 0x007F005D; // red
        // return 0x00007F00; // green
        return 0x000015E2; // blue
        // return 0x00FF0700; // green
        // return 0x00FFF700; // green more bright
        // return color;
        // return 0x00001BC3; // another green
        // return 0x00FF00FF; // purple
        // return 0x00005DFF;  // Y
        // return 0xFF002F00; // yellow
        // return 0xFFFFFFFF; // white more bright
    }
    else
    {
        // return 0x000009C6;
        return 0x00007f00;
        // return 0;
    }
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    int k;
    SystemCoreClockUpdate();
    // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk: %d MHz\r\n", SystemCoreClock / 1000000);
    printf("Chip ID: %08X\r\n", DBGMCU_GetCHIPID());

    WS2812BDMAInit();
    // WS2812BDMAStart(NR_LEDS);

    printf("WS2812BDMAInit OK\r\n");

    frameno = 0;

    for (k = 0; k < NR_LEDS; k++)
        phases[k] = k << 8;

    int tweendir = 0;

    while (1)
    {
        // GPIOD->BSHR = 1;     // Turn on GPIOD0
        //  Wait for LEDs to totally finish.
        // Delay_Ms( 12 );
        // GPIOD->BSHR = 1<<16; // Turn it off

        printf("WS2812BLEDInUse\r\n");

        while (WS2812BLEDInUse)
            ;

        frameno++;

        if (frameno == 1024)
        {
            tweendir = 1;
        }
        if (frameno == 2048)
        {
            tweendir = -1;
            frameno = 0;
        }

        if (tweendir)
        {
            int t = tween + tweendir;
            if (t > 255)
                t = 255;
            if (t < -NR_LEDS)
                t = -NR_LEDS;
            tween = t;
        }

        for (k = 0; k < NR_LEDS; k++)
        {
            phases[k] += ((((rands[k & 0xff]) + 0xf) << 2) + (((rands[k & 0xff]) + 0xf) << 1)) >> 1;
        }

        printf("WS2812BDMAStart\r\n");
        WS2812BDMAStart(NR_LEDS);
        // printf("WS2812BDMAStart OK\r\n");
        // Delay_Ms(10000);
        Delay_Ms(500);
        // Delay_Ms(100);
    }
}
