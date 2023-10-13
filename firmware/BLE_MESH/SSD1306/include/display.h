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
#define DISPLAY_TEST_EVENT 0x0001
#define DISPLAY_CLEAR_EVENT 0x0002
#define DISPLAY_SHOW_MENU_EVENT 0x0004
#define DISPLAY_SHOW_LOGO_EVENT 0x0008

#define NO_DELAY 0
#define SHOW_LOGO_DELAY 1000
#define SHOW_MENU_DELAY 1200

#define WHITE 1
#define BLACK 0

#define LAYER_MAIN 0
#define LAYER_NEOPIXELS 1

/* Function protopotypes */
tmosEvents Display_ProcessEvent(tmosTaskID task_id, tmosEvents events);
void IIC_Init(u32 bound, u16 address);
void Scan_I2C_Devices();
void Display_Test();
// Initialize the display
void Display_Init();
void Display_Clear();
void Display_Show_Logo();
void Display_Show_Menu();
void Display_Update_Menu();

/* Global variables */
extern tmosTaskID displayTaskID;
extern uint8_t selectedOption;
extern uint8_t previousLayer;
extern uint8_t currentLayer;
extern uint8_t optionsSize;

#endif // DISPLAY_H
