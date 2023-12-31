#ifndef FLASH_H
#define FLASH_H

// #include "HAL.h"
#include "debug.h"

/* Global define */
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
#define PAGE_WRITE_START_ADDR  ((uint32_t)0x08068000) /* Start from 32K */
#define PAGE_WRITE_END_ADDR    ((uint32_t)0x08069000) /* End at 36K */
#define FLASH_PAGE_SIZE                   4096
#define FLASH_PAGES_TO_BE_PROTECTED FLASH_WRProt_Pages60to63

/* Fast Mode define */
#define FAST_FLASH_PROGRAM_START_ADDR  ((uint32_t)0x08008000)
#define FAST_FLASH_PROGRAM_END_ADDR  ((uint32_t)0x08010000)
#define FAST_FLASH_SIZE  (64*1024)

/* Reboot define */
#define REBOOT_COUNTER_ADDRESS ((uint32_t)0x0806A000)
#define REBOOT_COUNTER_ADDRESS_FLAG ((uint32_t)0x0806B000)

void Flash_Init();
void Flash_Erase();
uint32_t Flash_Get_Reboot_Counter();
void Flash_Test(void);
void Flash_Test_Fast(void);

#endif