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
 *FLASH erase/read/write, and fast programming:
 *    Includes Standard Erase and Program, Fast Erase and Program.
 *
 *
 *    a-Erase successful part read non-0xFF:
 *                    Word reading--0xe339e339
 *            Half-word reading--0xe339
 *                       byte read--0x39
 *            Even address byte read--0x39
 *            Odd address byte read--0xe3
 *    b-When If the actual application must require the use of the system with a frequency greater than 120M,
 *            attention should be paid to:
 *            In non-zero wait area FLASH and zero wait area FLASH, user word read and write, manufacturer
 *            configuration word and Boot area read, you need to do the following operations. Firstly, divide
 *            the frequency of HCLK by 2 (the related peripheral clocks are also divided at the same time, and
 *            the impact needs to be evaluated). After the FLASH operation is completed, restore it to ensure
 *            the FLASH access clock The frequency does not exceed 60Mhz (bit[25]-SCKMOD of the FLASH_CTLR register
 *            can configure the FLASH access clock frequency to be the system clock or half of the system clock,
 *            and this bit is configured as half of the system clock by default).
 *
 */

#include "debug.h"

/* Global define */
#define false 0
#define true 1

typedef enum
{
    FAILED = 0,
    PASSED = !FAILED
} TestStatus;
#define PAGE_WRITE_START_ADDR ((uint32_t)0x08008000) /* Start from 32K */
#define PAGE_WRITE_END_ADDR ((uint32_t)0x08009000)   /* End at 36K */
#define FLASH_PAGE_SIZE 4096
#define FLASH_PAGES_TO_BE_PROTECTED FLASH_WRProt_Pages60to63

/* Fast Mode define */
#define FAST_FLASH_PROGRAM_START_ADDR ((uint32_t)0x08008000)
#define FAST_FLASH_PROGRAM_END_ADDR ((uint32_t)0x08010000)
#define FAST_FLASH_SIZE (64 * 1024)

#define REBOOT_COUNTER_ADDRESS ((uint32_t)0x08008000)
// #define REBOOT_COUNTER_ADDRESS_FLAG ((uint32_t)0x0800A004)
#define REBOOT_COUNTER_ADDRESS_FLAG ((uint32_t)0x08009000)

/* Global Variable */
uint32_t rebootCounter = 0;
uint32_t rebootCounterFlag = 0;
FLASH_Status FLASHStatus = FLASH_COMPLETE;

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);
    // printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf("Flash Program Test\r\n");

    FLASH_Unlock();
    // Attempt to read rebootCounterFlag from flash
    rebootCounterFlag = *((uint32_t *)REBOOT_COUNTER_ADDRESS_FLAG);
    printf("Reboot Counter Flag: %d\r\n", rebootCounterFlag);

    // Check if the value is not valid (initialized)
    if (rebootCounterFlag != true)
    {
        // Initialize the value
        printf("Reboot Counter Flag is not valid\r\n");
        FLASHStatus = FLASH_ErasePage(REBOOT_COUNTER_ADDRESS);
        rebootCounter = 0;
        rebootCounterFlag = true;
        FLASHStatus = FLASH_ProgramWord(REBOOT_COUNTER_ADDRESS, rebootCounter);
        FLASHStatus = FLASH_ProgramWord(REBOOT_COUNTER_ADDRESS_FLAG, rebootCounterFlag);
    }
    else if (rebootCounterFlag)
    {
        rebootCounter = *((uint32_t *)REBOOT_COUNTER_ADDRESS);
    }

    rebootCounter++;
    printf("Reboot Counter: %d\r\n", rebootCounter);
    FLASHStatus = FLASH_ProgramWord(REBOOT_COUNTER_ADDRESS, rebootCounter);

    // Erase the page
    // FLASHStatus = FLASH_ErasePage(REBOOT_COUNTER_ADDRESS_FLAG);

    FLASH_Lock();

    printf("Finish!\r\n");

    while (1)
    {
    }
}
