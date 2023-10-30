#ifndef FLASH_H
#define FLASH_H

// #include "HAL.h"
#include "debug.h"

void Flash_Init();
void Flash_Erase();
uint32_t Flash_Get_Reboot_Counter();

#endif