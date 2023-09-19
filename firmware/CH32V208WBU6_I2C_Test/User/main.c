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
//#include "i2c_oled.h"
#include "ch32v003fun.h"
#include <stdio.h>
#include "ssd1306_i2c.h"
#include "ssd1306.h"
#include "bomb.h"

/* I2C Mode Definition */
#define I2C_TIMEOUT  1000

/* Global define */
#define TxAdderss   0x02

/* Function protopotypes */
void IIC_Init(u32 bound, u16 address);
void Scan_I2C_Devices();
void oled_test();

/* Global Variable */

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
    // printf("SystemClk:%d\r\n", SystemCoreClock);
    IIC_Init(80000, TxAdderss);
    Scan_I2C_Devices();
    oled_test();

    while(1);
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
    GPIO_InitTypeDef GPIO_InitStructure={0};
    I2C_InitTypeDef I2C_InitTSturcture={0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE );
    // GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_I2C2, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

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

void Scan_I2C_Devices() {
    printf("Scanning I2C bus...\n");

    for (uint8_t address = 1; address < 128; address++) {
        uint32_t timeout = I2C_TIMEOUT;
        while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY)) {
            if (--timeout == 0) {
                printf("I2C bus is busy\n");
                return;
            }
        }
        I2C_GenerateSTART(I2C2, ENABLE);
        timeout = I2C_TIMEOUT;
        while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)) {
            if (--timeout == 0) {
                printf("Failed to generate start condition\n");
                return;
            }
        }
        I2C_Send7bitAddress(I2C2, address << 1, I2C_Direction_Transmitter);
        timeout = I2C_TIMEOUT;
        while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
            if (--timeout == 0) {
                // printf("No response from address 0x%02X\n", address);
                break;
            }
        }
        I2C_GenerateSTOP(I2C2, ENABLE);
        if (timeout > 0) {
            printf("Found device at address 0x%02X\n", address);
        }
    }
}

void oled_test()
{
    // 48MHz internal clock
    SystemInit();

    Delay_Ms( 100 );
    printf("\r\r\n\ni2c_oled example\n\r");

    // init i2c and oled
    Delay_Ms( 100 );    // give OLED some more time
    printf("initializing i2c oled...");
    if(!ssd1306_i2c_init())
    {
        ssd1306_init();
        printf("done.\n\r");

        printf("Looping on test modes...");
        while(1)
        {
            for(uint8_t mode=0;mode<(SSD1306_H>32?9:8);mode++)
            {
                // clear buffer for next mode
                ssd1306_setbuf(0);

                switch(mode)
                {
                    case 0:
                        printf("buffer fill with binary\n\r");
                        for(int i=0;i<sizeof(ssd1306_buffer);i++)
                            ssd1306_buffer[i] = i;
                        break;

                    case 1:
                        printf("pixel plots\n\r");
                        for(int i=0;i<SSD1306_W;i++)
                        {
                            ssd1306_drawPixel(i, i/(SSD1306_W/SSD1306_H), 1);
                            ssd1306_drawPixel(i, SSD1306_H-1-(i/(SSD1306_W/SSD1306_H)), 1);
                        }
                        break;

                    case 2:
                        {
                            printf("Line plots\n\r");
                            uint8_t y= 0;
                            for(uint8_t x=0;x<SSD1306_W;x+=16)
                            {
                                ssd1306_drawLine(x, 0, SSD1306_W, y, 1);
                                ssd1306_drawLine(SSD1306_W-x, SSD1306_H, 0, SSD1306_H-y, 1);
                                y+= SSD1306_H/8;
                            }
                        }
                        break;

                    case 3:
                        printf("Circles empty and filled\n\r");
                        for(uint8_t x=0;x<SSD1306_W;x+=16)
                            if(x<64)
                                ssd1306_drawCircle(x, SSD1306_H/2, 15, 1);
                            else
                                ssd1306_fillCircle(x, SSD1306_H/2, 15, 1);
                        break;
                    case 4:
                        printf("Image\n\r");
                        ssd1306_drawImage(0, 0, bomb_i_stripped, 32, 32, 0);
                        break;
                    case 5:
                        printf("Unscaled Text\n\r");
                        ssd1306_drawstr(0,0, "This is a test", 1);
                        ssd1306_drawstr(0,8, "of the emergency", 1);
                        ssd1306_drawstr(0,16,"broadcasting", 1);
                        ssd1306_drawstr(0,24,"system.",1);
                        if(SSD1306_H>32)
                        {
                            ssd1306_drawstr(0,32, "Lorem ipsum", 1);
                            ssd1306_drawstr(0,40, "dolor sit amet,", 1);
                            ssd1306_drawstr(0,48,"consectetur", 1);
                            ssd1306_drawstr(0,56,"adipiscing",1);
                        }
                        ssd1306_xorrect(SSD1306_W/2, 0, SSD1306_W/2, SSD1306_W);
                        break;

                    case 6:
                        printf("Scaled Text 1, 2\n\r");
                        ssd1306_drawstr_sz(0,0, "sz 8x8", 1, fontsize_8x8);
                        ssd1306_drawstr_sz(0,16, "16x16", 1, fontsize_16x16);
                        break;

                    case 7:
                        printf("Scaled Text 4\n\r");
                        ssd1306_drawstr_sz(0,0, "32x32", 1, fontsize_32x32);
                        break;


                    case 8:
                        printf("Scaled Text 8\n\r");
                        ssd1306_drawstr_sz(0,0, "64", 1, fontsize_64x64);
                        break;

                    default:
                        break;
                }
                ssd1306_refresh();

                Delay_Ms(2000);
            }
        }
    }
    else
        printf("failed.\n\r");

    printf("Stuck here forever...\n\r");
    while(1);
}
