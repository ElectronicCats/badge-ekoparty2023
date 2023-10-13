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
    {
        APP_DBG("Button pressed: BACK");
        break;
    }
    case BUTTON_UP:
    {
        APP_DBG("Button pressed: UP");
        break;
    }
    case BUTTON_DOWN:
    {
        APP_DBG("Button pressed: DOWN");
        break;
    }
    case BUTTON_SELECT:
    {
        APP_DBG("Button pressed: SELECT");
        break;
    }
    default:
    {
        break;
    }
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
        previousLayer = currentLayer;
        switch (currentLayer)
        {
        case LAYER_MAIN:
            Main_Options();
            break;
        }
        break;
    }
    default:
    {
        break;
    }
    }

    APP_DBG("selectedOption %d", selectedOption);
}

void Main_Options()
{
    switch (selectedOption)
    {
    case 0:
    {
        currentLayer = LAYER_NEOPIXELS;
        Display_Update_Menu();
        break;
    }
    case 1:
    {
        // currentLayer = LAYER_BLE_MESH;
        // Display_Clear();
        // Display_Show_Ble_Mesh_Menu();
        break;
    }
    case 2:
    {
        // currentLayer = LAYER_SETTINGS;
        // Display_Clear();
        // Display_Show_Settings_Menu();
        break;
    }
    default:
    {
        break;
    }
    }
}
