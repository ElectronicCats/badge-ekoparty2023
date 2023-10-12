#ifndef DISPLAY_H
#define DISPLAY_H

#include "app_mesh_config.h"
// #include "ch32v003fun.h"
#include <stdio.h>
#include "ssd1306_i2c.h"
#include "ssd1306.h"
#include "bomb.h"
#include "HAL.h"

#define I2C_TIMEOUT 1000
#define TxAdderss 0x02
#define DISPLAY_TEST_EVENT 0x0005

/* Function protopotypes */
tmosEvents Display_ProcessEvent(tmosTaskID task_id, tmosEvents events);
void IIC_Init(u32 bound, u16 address);
void Scan_I2C_Devices();
void Oled_Test();
// Initialize the display
void Display_Init();
// Clear the display
void Display_Clear();

/* Global variables */
extern tmosTaskID displayTaskID;

#endif // DISPLAY_H
