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
 *7-bit addressing mode, master/slave mode, transceiver routine:
 *I2C2_SCL(PB10)\I2C2_SDA(PB11).
 *This routine demonstrates that Master sends and Slave receives.
 *Note: The two boards download the Master and Slave programs respectively,
 *and power on at the same time.
 *    Hardware connection:PB8 -- PB8
 *                        PB9 -- PB9
 *
 */

#include "debug.h"
#include <stdio.h>
#include <string.h>
#include "ch32v003fun.h"
#include "ssd1306_i2c.h"
#include "ssd1306.h"
#include "bomb.h"
#include "color_utilities.h"

/* Global define */
#define I2C_TIMEOUT 1000
#define TxAdderss 0x02
#define WS2812DMA_IMPLEMENTATION
// #define WSRBG //For WS2816C's.
#define WSGRB // For SK6805-EC15
#define LEDS_NUM 5

#include "ws2812b_dma_spi_led_driver.h"

/* Function protopotypes */
void IIC_Init(u32 bound, u16 address);
void Scan_I2C_Devices();
void oled_test();
uint32_t WS2812BLEDCallback(int ledno);

/* Global Variable */
uint16_t phases[LEDS_NUM];
int frameno;
volatile int tween = -LEDS_NUM;

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk: %d MHz\r\n", SystemCoreClock / 1000000);
    IIC_Init(80000, TxAdderss);
    Scan_I2C_Devices();

    // 48MHz internal clock
    SystemInit();
    Delay_Ms(100); // give OLED some more time
    printf("initializing i2c oled...\n");
    ssd1306_init(); // Shows the last image before power off
    printf("done.\n\r");
    oled_test();

    WS2812BDMAInit();
    frameno = 1015;
    int k;

    for (k = 0; k < LEDS_NUM; k++)
        phases[k] = k << 8;

    int tweendir = 0;

    while (1)
    {
        // GPIOD->BSHR = 1;     // Turn on GPIOD0
        //  Wait for LEDs to totally finish.
        // Delay_Ms( 12 );
        // GPIOD->BSHR = 1<<16; // Turn it off

        // printf("WS2812BLEDInUse\r\n");

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
            {
                t = 255;
            }
            if (t < -LEDS_NUM)
            {
                t = -LEDS_NUM;
            }

            tween = t;
        }

        for (k = 0; k < LEDS_NUM; k++)
        {
            phases[k] += ((((rands[k & 0xff]) + 0xf) << 2) + (((rands[k & 0xff]) + 0xf) << 1)) >> 1;
            printf("Phases: %d\n", phases[k]);
        }

        printf("WS2812BDMAStart\r\n");
        printf("tweendir: %d\n", tweendir);
        printf("tween: %d\n", tween);
        WS2812BDMAStart(LEDS_NUM);
        // printf("WS2812BDMAStart OK\r\n");
        Delay_Ms(2000);
    }
}

/*********************************************************************
 * @fn      IIC_Init
 *
 * @brief   Initializes the IIC peripheral.
 *
 * @return  none
 */
void IIC_Init(u32 bound, u16 address)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2C_InitTypeDef I2C_InitTSturcture = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    // GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_InitTSturcture.I2C_ClockSpeed = bound;
    I2C_InitTSturcture.I2C_Mode = I2C_Mode_I2C;
    I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_16_9;
    I2C_InitTSturcture.I2C_OwnAddress1 = address;
    I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
    I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C2, &I2C_InitTSturcture);

    I2C_Cmd(I2C2, ENABLE);
    I2C_AcknowledgeConfig(I2C2, ENABLE);
}

void Scan_I2C_Devices()
{
    printf("Scanning I2C bus...\n");
    uint8_t found = 0;

    for (uint8_t address = 1; address < 128; address++)
    {
        uint32_t timeout = I2C_TIMEOUT;
        while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY))
        {
            if (--timeout == 0)
            {
                printf("I2C bus is busy\n");
                return;
            }
        }
        I2C_GenerateSTART(I2C2, ENABLE);
        timeout = I2C_TIMEOUT;
        while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
        {
            if (--timeout == 0)
            {
                printf("Failed to generate start condition\n");
                return;
            }
        }
        I2C_Send7bitAddress(I2C2, address << 1, I2C_Direction_Transmitter);
        timeout = I2C_TIMEOUT;
        while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        {
            if (--timeout == 0)
            {
                // printf("No response from address 0x%02X\n", address);
                break;
            }
        }
        I2C_GenerateSTOP(I2C2, ENABLE);
        if (timeout > 0)
        {
            printf("Found device at address 0x%02X\r\n", address);
            found++;
            break;
        }
    }

    if (found == 0)
    {
        printf("No I2C devices found\n");
        printf("Wait for ever...\n");
        while (1)
        {
            Delay_Ms(1000);
        }
    }
}

void oled_test()
{
    printf("Looping on test modes...\n");
    ssd1306_setbuf(0);
    ssd1306_drawstr(0, 32, "Electronic Cats", 1);
    ssd1306_xorrect(0, 0, SSD1306_W / 2, SSD1306_W);
    ssd1306_refresh();

    while (0)
    {
        for (uint8_t mode = 0; mode < (SSD1306_H > 32 ? 9 : 8); mode++)
        {
            // clear buffer for next mode
            ssd1306_setbuf(0);

            switch (mode)
            {
            case 0:
                printf("buffer fill with binary\n\r");
                for (int i = 0; i < sizeof(ssd1306_buffer); i++)
                    ssd1306_buffer[i] = i;
                break;

            case 1:
                printf("pixel plots\n\r");
                for (int i = 0; i < SSD1306_W; i++)
                {
                    ssd1306_drawPixel(i, i / (SSD1306_W / SSD1306_H), 1);
                    ssd1306_drawPixel(i, SSD1306_H - 1 - (i / (SSD1306_W / SSD1306_H)), 1);
                }
                break;

            case 2:
            {
                printf("Line plots\n\r");
                uint8_t y = 0;
                for (uint8_t x = 0; x < SSD1306_W; x += 16)
                {
                    ssd1306_drawLine(x, 0, SSD1306_W, y, 1);
                    ssd1306_drawLine(SSD1306_W - x, SSD1306_H, 0, SSD1306_H - y, 1);
                    y += SSD1306_H / 8;
                }
            }
            break;

            case 3:
                printf("Circles empty and filled\n\r");
                for (uint8_t x = 0; x < SSD1306_W; x += 16)
                    if (x < 64)
                        ssd1306_drawCircle(x, SSD1306_H / 2, 15, 1);
                    else
                        ssd1306_fillCircle(x, SSD1306_H / 2, 15, 1);
                break;
            case 4:
                printf("Image\n\r");
                ssd1306_drawImage(0, 0, bomb_i_stripped, 32, 32, 0);
                break;
            case 5:
                printf("Unscaled Text\n\r");
                ssd1306_drawstr(0, 0, "This is a test", 1);
                ssd1306_drawstr(0, 8, "of the emergency", 1);
                ssd1306_drawstr(0, 16, "broadcasting", 1);
                ssd1306_drawstr(0, 24, "system.", 1);
                if (SSD1306_H > 32)
                {
                    ssd1306_drawstr(0, 32, "Lorem ipsum", 1);
                    ssd1306_drawstr(0, 40, "dolor sit amet,", 1);
                    ssd1306_drawstr(0, 48, "consectetur", 1);
                    ssd1306_drawstr(0, 56, "adipiscing", 1);
                }
                ssd1306_xorrect(SSD1306_W / 2, 0, SSD1306_W / 2, SSD1306_W);
                break;

            case 6:
                printf("Scaled Text 1, 2\n\r");
                ssd1306_drawstr_sz(0, 0, "sz 8x8", 1, fontsize_8x8);
                ssd1306_drawstr_sz(0, 16, "16x16", 1, fontsize_16x16);
                break;

            case 7:
                printf("Scaled Text 4\n\r");
                ssd1306_drawstr_sz(0, 0, "32x32", 1, fontsize_32x32);
                break;

            case 8:
                printf("Scaled Text 8\n\r");
                ssd1306_drawstr_sz(0, 0, "64", 1, fontsize_64x64);
                break;

            default:
                break;
            }

            ssd1306_refresh();
            Delay_Ms(2000);

            if (mode == 8)
            {
                break;
            }
        }

        break;
    }
}

// Callbacks that you must implement.
uint32_t WS2812BLEDCallback(int ledno)
{
    uint8_t index = (phases[ledno]) >> 8;
    printf("Index: %d\n", index);
    uint8_t rsbase = sintable[index];
    uint8_t rs = rsbase >> 3;
    uint32_t fire = ((huetable[(rs + 190) & 0xff] >> 1) << 16) | (huetable[(rs + 30) & 0xff]) | ((huetable[(rs + 0)] >> 1) << 8);
    uint32_t ice = 0x7f0000 | ((rsbase >> 1) << 8) | ((rsbase >> 1));

    // Because this chip doesn't natively support multiplies, we are going to avoid tweening of 1..254.
    uint32_t tweenHexColors = TweenHexColors(fire, ice, ((tween + ledno) > 0) ? 255 : 0); // Where "tween" is a value from 0 ... 255
    printf("Tween hex colors: 0x%06X\n", tweenHexColors);
    printf("tween: %d\n", tween);
    // return tweenHexColors;
    return ice;
}
