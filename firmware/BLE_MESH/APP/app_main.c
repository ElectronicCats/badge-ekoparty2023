/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2022/06/22
 * Description        :
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/******************************************************************************/
/* Header file contains */
#include "CONFIG.h"
#include "MESH_LIB.h"
#include "HAL.h"
#include "app_mesh_config.h"
#include "app.h"
#include "ch32v003fun.h"
#include <stdio.h>
#include "ssd1306_i2c.h"
#include "ssd1306.h"
#include "bomb.h"

/* I2C Mode Definition */
#define I2C_TIMEOUT 1000

/* Global define */
#define TxAdderss 0x02

/* Function protopotypes */
void IIC_Init(u32 bound, u16 address);
void Scan_I2C_Devices();
void oled_test();

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
__attribute__((aligned(4))) uint32_t MEM_BUF[BLE_MEMHEAP_SIZE / 4];
#if (defined(BLE_MAC)) && (BLE_MAC == TRUE)
const uint8_t MacAddr[6] = {0x84, 0xC2, 0xE4, 0x03, 0x02, 0x02};
#endif

/*********************************************************************
 * @fn      bt_mesh_lib_init
 *
 * @brief   mesh ���ʼ��
 *
 * @return  state
 */
uint8_t bt_mesh_lib_init(void)
{
    uint8_t ret;

    if (tmos_memcmp(VER_MESH_LIB, VER_MESH_FILE, strlen(VER_MESH_FILE)) == FALSE)
    {
        APP_DBG("mesh head file error...");
        while (1)
            ;
    }

    ret = RF_RoleInit();

#if ((CONFIG_BLE_MESH_PROXY) ||   \
     (CONFIG_BLE_MESH_PB_GATT) || \
     (CONFIG_BLE_MESH_OTA))
    ret = GAPRole_PeripheralInit();
#endif /* PROXY || PB-GATT || OTA */

#if (CONFIG_BLE_MESH_PROXY_CLI)
    ret = GAPRole_CentralInit();
#endif /* CONFIG_BLE_MESH_PROXY_CLI */

    MeshTimer_Init();
    MeshDeamon_Init();
    ble_sm_alg_ecc_init();

#if (CONFIG_BLE_MESH_IV_UPDATE_TEST)
    bt_mesh_iv_update_test(TRUE);
#endif
    return ret;
}

/*********************************************************************
 * @fn      Main_Circulation
 *
 * @brief   Main loop
 *
 * @return  none
 */
__attribute__((section(".highcode")))
__attribute__((noinline)) void
Main_Circulation(void)
{
    while (1)
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
    USART_APP_DBG_Init(115200);
#endif

    IIC_Init(80000, TxAdderss);

    // 48MHz internal clock
    SystemInit();
    // init i2c and oled
    Delay_Ms(100); // give OLED some more time
    Scan_I2C_Devices();
    APP_DBG("initializing i2c oled...");
    ssd1306_init();
    APP_DBG("Done.");
    oled_test();

    APP_DBG("%s", VER_LIB);
    WCHBLE_Init();
    HAL_Init();
    bt_mesh_lib_init();
    App_Init();
    Main_Circulation();
}

/*********************************************************************
 * @fn      IIC_Init
 *
 * @brief   Initializes the IIC peripheral.
 *
 * @return  none
 */
void IIC_Init(u32 bound, u16 address)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2C_InitTypeDef I2C_InitTSturcture = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    // GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_InitTSturcture.I2C_ClockSpeed = bound;
    I2C_InitTSturcture.I2C_Mode = I2C_Mode_I2C;
    I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_16_9;
    I2C_InitTSturcture.I2C_OwnAddress1 = address;
    I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
    I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C2, &I2C_InitTSturcture);

    I2C_Cmd(I2C2, ENABLE);
    I2C_AcknowledgeConfig(I2C2, ENABLE);
}

void Scan_I2C_Devices()
{
    APP_DBG("Scanning I2C bus...");
    uint8_t found = 0;

    for (uint8_t address = 1; address < 128; address++)
    {
        uint32_t timeout = I2C_TIMEOUT;
        while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY))
        {
            if (--timeout == 0)
            {
                APP_DBG("I2C bus is busy");
                return;
            }
        }
        I2C_GenerateSTART(I2C2, ENABLE);
        timeout = I2C_TIMEOUT;
        while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
        {
            if (--timeout == 0)
            {
                APP_DBG("Failed to generate start condition");
                return;
            }
        }
        I2C_Send7bitAddress(I2C2, address << 1, I2C_Direction_Transmitter);
        timeout = I2C_TIMEOUT;
        while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        {
            if (--timeout == 0)
            {
                // APP_DBG("No response from address 0x%02X", address);
                break;
            }
        }
        I2C_GenerateSTOP(I2C2, ENABLE);
        if (timeout > 0)
        {
            APP_DBG("Found device at address 0x%02X", address);
            found++;
            break;
        }
    }

    if (found == 0)
    {
        APP_DBG("No I2C devices found");
        APP_DBG("Wait for ever...");
        while (1)
        {
            Delay_Ms(1000);
        }
    }
}

void oled_test()
{
    APP_DBG("Looping on test modes...");

    while (1)
    {
        for (uint8_t mode = 0; mode < (SSD1306_H > 32 ? 9 : 8); mode++)
        {
            // clear buffer for next mode
            ssd1306_setbuf(0);

            switch (mode)
            {
            case 0:
                APP_DBG("buffer fill with binary");
                for (int i = 0; i < sizeof(ssd1306_buffer); i++)
                    ssd1306_buffer[i] = i;
                break;

            case 1:
                APP_DBG("pixel plots");
                for (int i = 0; i < SSD1306_W; i++)
                {
                    ssd1306_drawPixel(i, i / (SSD1306_W / SSD1306_H), 1);
                    ssd1306_drawPixel(i, SSD1306_H - 1 - (i / (SSD1306_W / SSD1306_H)), 1);
                }
                break;

            case 2:
            {
                APP_DBG("Line plots");
                uint8_t y = 0;
                for (uint8_t x = 0; x < SSD1306_W; x += 16)
                {
                    ssd1306_drawLine(x, 0, SSD1306_W, y, 1);
                    ssd1306_drawLine(SSD1306_W - x, SSD1306_H, 0, SSD1306_H - y, 1);
                    y += SSD1306_H / 8;
                }
            }
            break;

            case 3:
                APP_DBG("Circles empty and filled");
                for (uint8_t x = 0; x < SSD1306_W; x += 16)
                    if (x < 64)
                        ssd1306_drawCircle(x, SSD1306_H / 2, 15, 1);
                    else
                        ssd1306_fillCircle(x, SSD1306_H / 2, 15, 1);
                break;
            case 4:
                APP_DBG("Image");
                ssd1306_drawImage(0, 0, bomb_i_stripped, 32, 32, 0);
                break;
            case 5:
                APP_DBG("Unscaled Text");
                ssd1306_drawstr(0, 0, "This is a test", 1);
                ssd1306_drawstr(0, 8, "of the emergency", 1);
                ssd1306_drawstr(0, 16, "broadcasting", 1);
                ssd1306_drawstr(0, 24, "system.", 1);
                if (SSD1306_H > 32)
                {
                    ssd1306_drawstr(0, 32, "Lorem ipsum", 1);
                    ssd1306_drawstr(0, 40, "dolor sit amet,", 1);
                    ssd1306_drawstr(0, 48, "consectetur", 1);
                    ssd1306_drawstr(0, 56, "adipiscing", 1);
                }
                ssd1306_xorrect(SSD1306_W / 2, 0, SSD1306_W / 2, SSD1306_W);
                break;

            case 6:
                APP_DBG("Scaled Text 1, 2");
                ssd1306_drawstr_sz(0, 0, "sz 8x8", 1, fontsize_8x8);
                ssd1306_drawstr_sz(0, 16, "16x16", 1, fontsize_16x16);
                break;

            case 7:
                APP_DBG("Scaled Text 4");
                ssd1306_drawstr_sz(0, 0, "32x32", 1, fontsize_32x32);
                break;

            case 8:
                APP_DBG("Scaled Text 8");
                ssd1306_drawstr_sz(0, 0, "64", 1, fontsize_64x64);
                break;

            default:
                break;
            }

            ssd1306_refresh();
            Delay_Ms(2000);

            if (mode == 8) {
                break;
            }
        }

        break;
    }
}

/******************************** endfile @ main ******************************/
