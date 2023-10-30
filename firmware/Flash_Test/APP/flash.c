#include "flash.h"

#define REBOOT_COUNTER_ADDRESS ((uint32_t)0x0800A000)
#define REBOOT_COUNTER_ADDRESS_FLAG ((uint32_t)0x0800B000)

/* Private Variable */
static uint32_t rebootCounter = 0;
static uint32_t rebootCounterFlag = 0;
static FLASH_Status FLASHStatus = FLASH_COMPLETE;

void Flash_Init(void)
{
    FLASH_Unlock();
    // Attempt to read rebootCounterFlag from flash
    rebootCounterFlag = *((uint32_t *)REBOOT_COUNTER_ADDRESS_FLAG);
    // printf("Reboot Counter Flag: %d\r\n", rebootCounterFlag);

    // Check if the value is not valid (initialized)
    if (rebootCounterFlag != 1)
    {
        // Initialize the value
        // printf("Reboot Counter Flag is not valid\r\n");
        FLASHStatus = FLASH_ErasePage(REBOOT_COUNTER_ADDRESS);
        rebootCounter = 0;
        rebootCounterFlag = 1;
        FLASHStatus = FLASH_ProgramWord(REBOOT_COUNTER_ADDRESS, rebootCounter);
        FLASHStatus = FLASH_ProgramWord(REBOOT_COUNTER_ADDRESS_FLAG, rebootCounterFlag);
    }
    else if (rebootCounterFlag)
    {
        rebootCounter = *((uint32_t *)REBOOT_COUNTER_ADDRESS);
    }

    rebootCounter++;
    // printf("Reboot Counter: %d\r\n", rebootCounter);
    FLASHStatus = FLASH_ProgramWord(REBOOT_COUNTER_ADDRESS, rebootCounter);

    // Erase the page
    // FLASHStatus = FLASH_ErasePage(REBOOT_COUNTER_ADDRESS_FLAG);

    FLASH_Lock();
}

void Flash_Erase(void)
{
    FLASH_Unlock();
    // FLASHStatus = FLASH_ErasePage(REBOOT_COUNTER_ADDRESS);
    FLASHStatus = FLASH_ErasePage(REBOOT_COUNTER_ADDRESS_FLAG);
    FLASH_Lock();
}

uint32_t Flash_Get_Reboot_Counter()
{
    return rebootCounter;
}
