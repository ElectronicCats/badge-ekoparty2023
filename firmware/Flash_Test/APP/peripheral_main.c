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

/******************************************************************************/
/* Header file contains */
#include "CONFIG.h"
#include "HAL.h"
#include "gattprofile.h"
#include "peripheral.h"
#include "flash.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
__attribute__((aligned(4))) uint32_t MEM_BUF[BLE_MEMHEAP_SIZE / 4];

#if(defined(BLE_MAC)) && (BLE_MAC == TRUE)
const uint8_t MacAddr[6] = {0x84, 0xC2, 0xE4, 0x03, 0x02, 0x02};
#endif

/*********************************************************************
 * @fn      Main_Circulation
 *
 * @brief   Main loop
 *
 * @return  none
 */
__attribute__((section(".highcode")))
__attribute__((noinline))
void Main_Circulation(void)
{
    while(1)
    {
        TMOS_SystemProcess();
    }
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main function
 *
 * @return  none
 */
int main(void)
{
    SystemCoreClockUpdate();
    Delay_Init();
#ifdef DEBUG
    USART_Printf_Init( 115200 );
#endif
    PRINT("%s\r\n", VER_LIB);

    printf("Flash Program Test\r\n");
    // Flash_Test();
    Flash_Init();
    printf("Reboot Counter: %d\r\r\n", Flash_Get_Reboot_Counter());

    printf("1\r\n");
    WCHBLE_Init();
    printf("2\r\n");
    HAL_Init();
    printf("3\r\n");
    GAPRole_PeripheralInit();
    printf("4\r\n");
    Peripheral_Init();

    Flash_Test();
    printf("Finished\r\n");

    Main_Circulation();
}

/******************************** endfile @ main ******************************/
