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

/* I2C Mode Definition */
#define I2C_TIMEOUT  1000

/* Global define */
#define TxAdderss   0x02

/* Global Variable */

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

    while(1);
}
