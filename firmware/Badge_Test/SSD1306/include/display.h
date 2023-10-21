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
#define LAYER_NEOPIXELS_MENU 1
#define LAYER_NEOPIXEL_1 2
#define LAYER_NEOPIXEL_2 3
#define LAYER_NEOPIXEL_3 4
#define LAYER_NEOPIXELS_RAINBOW 5

// Main menu options
#define NEOPIXELS_MENU 0

// Neopixels menu options
#define NEOPIXEL_1 0
#define NEOPIXEL_2 1
#define NEOPIXEL_3 2
#define NEOPIXELS_RAINBOW 3

// Neopixel options
#define NEOPIXEL_SET_RED 0
#define NEOPIXEL_SET_GREEN 1
#define NEOPIXEL_SET_BLUE 2
#define NEOPIXEL_ON 3
#define NEOPIXEL_OFF 4

/* Global variables */
extern tmosTaskID displayTaskID;
extern uint8_t selectedOption;
extern uint8_t previousLayer;
extern uint8_t currentLayer;
extern uint8_t optionsSize;

/* Function protopotypes */
tmosEvents Display_ProcessEvent(tmosTaskID task_id, tmosEvents events);
void IIC_Init(u32 bound, u16 address);
void Scan_I2C_Devices();
void Display_Test();
void Display_Init();
void Display_Clear();
void Display_Show_Logo();
void Display_Show_Menu();
void Display_Update_Menu();
char **Display_Update_Menu_Options();

#endif // DISPLAY_H