#include "Keyboard.h"

#define WS2812DMA_IMPLEMENTATION
// #define WSRBG //For WS2816C's.
#define WSGRB // For SK6805-EC15
#include "leds.h"

void Keyboard_Print_Button(uint8_t keys)
{
    switch (keys)
    {
    case BUTTON_BACK:
        APP_DBG("Button pressed: BACK");
        break;
    case BUTTON_UP:
        APP_DBG("Button pressed: UP");
        break;
    case BUTTON_DOWN:
        APP_DBG("Button pressed: DOWN");
        break;
    case BUTTON_SELECT:
        APP_DBG("Button pressed: SELECT");
        break;
    default:
        break;
    }
}

void Keyboard_Print_Layer(uint8_t layer)
{
    switch (layer)
    {
    case LAYER_MAIN:
        APP_DBG("Layer main");
        break;
    case LAYER_NEOPIXELS_MENU:
        APP_DBG("Layer neopixels menu");
        break;
    case LAYER_NEOPIXEL_1:
        APP_DBG("Layer neopixel 1");
        break;
    case LAYER_NEOPIXEL_2:
        APP_DBG("Layer neopixel 2");
        break;
    case LAYER_NEOPIXEL_3:
        APP_DBG("Layer neopixel 3");
        break;
    case LAYER_NEOPIXEL_RAINBOW:
        APP_DBG("Layer neopixels raibow");
        break;
    default:
        APP_DBG("Missing layer");
        break;
    }
}

void Keyboard_Scan_Callback(uint8_t keys)
{
    Keyboard_Print_Button(keys);

    switch (keys)
    {
    case BUTTON_BACK:
    {
        currentLayer = previousLayer;
        selectedOption = 0;
        Display_Update_Menu();
        break;
    }
    case BUTTON_UP:
    {
        selectedOption--;

        if (selectedOption == 255) // Underflow
        {
            selectedOption = 0;
        }

        Display_Update_Menu();
        break;
    }
    case BUTTON_DOWN:
    {
        selectedOption++;

        if (selectedOption > optionsSize - 1)
        {
            selectedOption = optionsSize - 1;
        }

        Display_Update_Menu();
        break;
    }
    case BUTTON_SELECT:
    {
        Update_Previous_Layer();

        switch (currentLayer)
        {
        case LAYER_MAIN:
            Main_Menu();
            break;
        case LAYER_NEOPIXELS_MENU:
            Neopixels_Menu();
            break;
        case LAYER_NEOPIXEL_1:
        case LAYER_NEOPIXEL_2:
        case LAYER_NEOPIXEL_3:
            Neopixel_Options();
            break;
        }
        break;
    }
    default:
    {
        break;
    }
    }

    Keyboard_Print_Layer(previousLayer);
    Keyboard_Print_Layer(currentLayer);
    APP_DBG("selectedOption %d", selectedOption);
}

void Update_Previous_Layer()
{
    uint8_t backupLayer = previousLayer;
    previousLayer = currentLayer;

    if (backupLayer != currentLayer)
    {
        previousLayer = backupLayer;
    }
}

void Main_Menu()
{
    switch (selectedOption)
    {
    case NEOPIXELS_MENU:
        currentLayer = LAYER_NEOPIXELS_MENU;
        Display_Update_Menu();
        break;
    case 1:
        // currentLayer = LAYER_BLE_MESH;
        // Display_Clear();
        // Display_Show_Ble_Mesh_Menu();
        break;
    case 2:
        // currentLayer = LAYER_SETTINGS;
        // Display_Clear();
        // Display_Show_Settings_Menu();
        break;
    default:
        APP_DBG("Missing option");
        break;
    }
}

void Neopixels_Menu()
{
    switch (selectedOption)
    {
    case NEOPIXEL_1:
        currentLayer = LAYER_NEOPIXEL_1;
        Display_Update_Menu();
        break;
    case NEOPIXEL_2:
        currentLayer = LAYER_NEOPIXEL_2;
        Display_Update_Menu();
        break;
    case NEOPIXEL_3:
        currentLayer = LAYER_NEOPIXEL_3;
        Display_Update_Menu();
        break;
    default:
        APP_DBG("Missing option");
        break;
    }
}

void Neopixel_Options()
{
    switch (selectedOption)
    {
    case NEOPIXEL_SET_RED:
        if (currentLayer == LAYER_NEOPIXEL_1)
        {
            led1_set_red();
        }
        else if (currentLayer == LAYER_NEOPIXEL_2)
        {
            led2_set_red();
        }
        else if (currentLayer == LAYER_NEOPIXEL_3)
        {
            led3_set_red();
        }
        break;
    case NEOPIXEL_SET_GREEN:
        if (currentLayer == LAYER_NEOPIXEL_1)
        {
            led1_set_green();
        }
        else if (currentLayer == LAYER_NEOPIXEL_2)
        {
            led2_set_green();
        }
        else if (currentLayer == LAYER_NEOPIXEL_3)
        {
            led3_set_green();
        }
        break;
    case NEOPIXEL_SET_BLUE:
        if (currentLayer == LAYER_NEOPIXEL_1)
        {
            led1_set_blue();
        }
        else if (currentLayer == LAYER_NEOPIXEL_2)
        {
            led2_set_blue();
        }
        else if (currentLayer == LAYER_NEOPIXEL_3)
        {
            led3_set_blue();
        }
        break;
    case NEOPIXEL_ON:
        if (currentLayer == LAYER_NEOPIXEL_1)
        {
            led1_on();
        }
        else if (currentLayer == LAYER_NEOPIXEL_2)
        {
            led2_on();
        }
        else if (currentLayer == LAYER_NEOPIXEL_3)
        {
            led3_on();
        }
        break;
    case NEOPIXEL_OFF:
        if (currentLayer == LAYER_NEOPIXEL_1)
        {
            led1_off();
        }
        else if (currentLayer == LAYER_NEOPIXEL_2)
        {
            led2_off();
        }
        else if (currentLayer == LAYER_NEOPIXEL_3)
        {
            led3_off();
        }
        break;
    default:
        APP_DBG("Missing option");
        break;
    }
}
