/********************************** (C) COPYRIGHT *******************************
 * File Name          : hidkbd.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/12/10
 * Description        : Bluetooth keyboard application, initialize the broadcast parameters,
 *                      and then broadcast until the host is connected, the key value is uploaded regularly
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "CONFIG.h"
#include "devinfoservice.h"
#include "battservice.h"
#include "hidkbdservice.h"
#include "hiddev.h"
#include "hidkbd.h"
#include "usb_hid_keys.h"
#include "app_mesh_config.h"
#include "gattprofile.h"

/*********************************************************************
 * MACROS
 */
// HID keyboard input report length
#define HID_KEYBOARD_IN_RPT_LEN 8

// HID LED output report length
#define HID_LED_OUT_RPT_LEN 1

/*********************************************************************
 * CONSTANTS
 */
// Param update delay
#define HID_START_PARAM_UPDATE_EVT_DELAY 12800

// Param update delay
#define START_PHY_UPDATE_DELAY 1600

// HID idle timeout in msec; set to zero to disable timeout
#define DEFAULT_HID_IDLE_TIMEOUT 60000

// Minimum connection interval (units of 1.25ms)
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL 8

// Maximum connection interval (units of 1.25ms)
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL 8

// Slave latency to use if parameter update request
#define DEFAULT_DESIRED_SLAVE_LATENCY 0

// Supervision timeout value (units of 10ms)
#define DEFAULT_DESIRED_CONN_TIMEOUT 500

// Default passcode
#define DEFAULT_PASSCODE 0

// Default GAP pairing mode
#define DEFAULT_PAIRING_MODE GAPBOND_PAIRING_MODE_WAIT_FOR_REQ

// Default MITM mode (TRUE to require passcode or OOB when pairing)
#define DEFAULT_MITM_MODE FALSE

// Default bonding mode, TRUE to bond
#define DEFAULT_BONDING_MODE TRUE

// Default GAP bonding I/O capabilities
#define DEFAULT_IO_CAPABILITIES GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT

// Battery level is critical when it is less than this %
#define DEFAULT_BATT_CRITICAL_LEVEL 6

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Task ID
static uint8_t hidEmuTaskId = INVALID_TASK_ID;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// GAP Profile - Name attribute for SCAN RSP data
static uint8_t scanRspData[] = {
    0x05, // length of this data
    GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE,
    LO_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL), // 100ms
    HI_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL),
    LO_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL), // 1s
    HI_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL),

    // service UUIDs
    0x05, // length of this data
    GAP_ADTYPE_16BIT_MORE,
    LO_UINT16(HID_SERV_UUID),
    HI_UINT16(HID_SERV_UUID),
    LO_UINT16(BATT_SERV_UUID),
    HI_UINT16(BATT_SERV_UUID),

    // Tx power level
    0x02, // length of this data
    GAP_ADTYPE_POWER_LEVEL,
    0 // 0dBm
};

// Advertising data
static uint8_t advertData[] = {
    // flags
    0x02, // length of this data
    GAP_ADTYPE_FLAGS,
    GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

    // appearance
    0x03, // length of this data
    GAP_ADTYPE_APPEARANCE,
    LO_UINT16(GAP_APPEARE_HID_KEYBOARD),
    HI_UINT16(GAP_APPEARE_HID_KEYBOARD),

    0x0D,                           // length of this data
    GAP_ADTYPE_LOCAL_NAME_COMPLETE, // AD Type = Complete local name
    'H',
    'I',
    'D',
    ' ',
    'K',
    'e',
    'y',
    'b',
    'o',
    'a',
    'r',
    'd', // connection interval range
};

// Device name attribute value
static const uint8_t attDeviceName[GAP_DEVICE_NAME_LEN] = "HID Keyboard";

// HID Dev configuration
static hidDevCfg_t hidEmuCfg = {
    DEFAULT_HID_IDLE_TIMEOUT, // Idle timeout
    HID_FEATURE_FLAGS         // HID feature flags
};

static uint16_t hidEmuConnHandle = GAP_CONNHANDLE_INIT;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void hidEmu_ProcessTMOSMsg(tmos_event_hdr_t *pMsg);
static void hidEmuSendKbdReport(uint8_t keycode);
static uint8_t hidEmuRcvReport(uint8_t len, uint8_t *pData);
static uint8_t hidEmuRptCB(uint8_t id, uint8_t type, uint16_t uuid,
                           uint8_t oper, uint16_t *pLen, uint8_t *pData);
static void hidEmuEvtCB(uint8_t evt);
static void hidEmuStateCB(gapRole_States_t newState, gapRoleEvent_t *pEvent);
static void hidSendMessage();
static uint8_t asciiToKeycode(char character);
static void simpleProfileChangeCB(uint8_t paramID, uint8_t *pValue, uint16_t len);

/*********************************************************************
 * PROFILE CALLBACKS
 */

static hidDevCB_t hidEmuHidCBs = {
    hidEmuRptCB,
    hidEmuEvtCB,
    NULL,
    hidEmuStateCB};

// Simple GATT Profile Callbacks
static simpleProfileCBs_t Peripheral_SimpleProfileCBs = {
    simpleProfileChangeCB // Characteristic value change callback
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      HidEmu_Init
 *
 * @brief   Initialization function for the HidEmuKbd App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by TMOS.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void HidEmu_Init()
{
    hidEmuTaskId = TMOS_ProcessEventRegister(HidEmu_ProcessEvent);

    // Setup the GAP Peripheral Role Profile
    {
        uint8_t initial_advertising_enable = TRUE;

        // Set the GAP Role Parameters
        GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &initial_advertising_enable);

        GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);
        GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData), scanRspData);
    }

    // Set the GAP Characteristics
    GGS_SetParameter(GGS_DEVICE_NAME_ATT, sizeof(attDeviceName), (void *)attDeviceName);

    // Set advertising interval
    {
        // Enable scan req notify
        GAP_SetParamValue(TGAP_ADV_SCAN_REQ_NOTIFY, ENABLE);
    }

    // Setup the GAP Bond Manager
    {
        uint32_t passkey = DEFAULT_PASSCODE; // passkey "000000"
        uint8_t pairMode = DEFAULT_PAIRING_MODE;
        uint8_t mitm = DEFAULT_MITM_MODE;
        uint8_t ioCap = DEFAULT_IO_CAPABILITIES;
        uint8_t bonding = DEFAULT_BONDING_MODE;
        GAPBondMgr_SetParameter(GAPBOND_PERI_DEFAULT_PASSCODE, sizeof(uint32_t), &passkey);
        GAPBondMgr_SetParameter(GAPBOND_PERI_PAIRING_MODE, sizeof(uint8_t), &pairMode);
        GAPBondMgr_SetParameter(GAPBOND_PERI_MITM_PROTECTION, sizeof(uint8_t), &mitm);
        GAPBondMgr_SetParameter(GAPBOND_PERI_IO_CAPABILITIES, sizeof(uint8_t), &ioCap);
        GAPBondMgr_SetParameter(GAPBOND_PERI_BONDING_ENABLED, sizeof(uint8_t), &bonding);
    }

    // Setup Battery Characteristic Values
    {
        uint8_t critical = DEFAULT_BATT_CRITICAL_LEVEL;
        Batt_SetParameter(BATT_PARAM_CRITICAL_LEVEL, sizeof(uint8_t), &critical);
    }

    // Initialize GATT attributes
    GGS_AddService(GATT_ALL_SERVICES);           // GAP
    GATTServApp_AddService(GATT_ALL_SERVICES);   // GATT attributes
    DevInfo_AddService();                        // Device Information Service
    SimpleProfile_AddService(GATT_ALL_SERVICES); // Simple GATT Profile

    // Setup the SimpleProfile Characteristic Values
    {
        uint8_t charValue1[SIMPLEPROFILE_CHAR1_LEN] = {1};
        uint8_t charValue2[SIMPLEPROFILE_CHAR2_LEN] = {2};
        uint8_t charValue3[SIMPLEPROFILE_CHAR3_LEN] = {3};
        uint8_t charValue4[SIMPLEPROFILE_CHAR4_LEN] = {4};
        uint8_t charValue5[SIMPLEPROFILE_CHAR5_LEN] = {1, 2, 3, 4, 5};

        SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR1, SIMPLEPROFILE_CHAR1_LEN, charValue1);
        SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR2, SIMPLEPROFILE_CHAR2_LEN, charValue2);
        SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR3, SIMPLEPROFILE_CHAR3_LEN, charValue3);
        SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR4, SIMPLEPROFILE_CHAR4_LEN, charValue4);
        SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR5, SIMPLEPROFILE_CHAR5_LEN, charValue5);
    }

    // Set up HID keyboard service
    Hid_AddService();

    // Register for HID Dev callback
    HidDev_Register(&hidEmuCfg, &hidEmuHidCBs);

    // Register callback with SimpleGATTprofile
    SimpleProfile_RegisterAppCBs(&Peripheral_SimpleProfileCBs);

    // Setup a delayed profile startup
    tmos_set_event(hidEmuTaskId, HID_START_DEVICE_EVT);
}

/*********************************************************************
 * @fn      HidEmu_ProcessEvent
 *
 * @brief   HidEmuKbd Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The TMOS assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
uint16_t HidEmu_ProcessEvent(uint8_t task_id, uint16_t events)
{
    if (events & SYS_EVENT_MSG)
    {
        uint8_t *pMsg;

        if ((pMsg = tmos_msg_receive(hidEmuTaskId)) != NULL)
        {
            hidEmu_ProcessTMOSMsg((tmos_event_hdr_t *)pMsg);

            // Release the TMOS message
            tmos_msg_deallocate(pMsg);
        }

        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }

    if (events & HID_START_DEVICE_EVT)
    {
        return (events ^ HID_START_DEVICE_EVT);
    }

    if (events & HID_START_PARAM_UPDATE_EVT)
    {
        // Send connect param update request
        GAPRole_PeripheralConnParamUpdateReq(hidEmuConnHandle,
                                             DEFAULT_DESIRED_MIN_CONN_INTERVAL,
                                             DEFAULT_DESIRED_MAX_CONN_INTERVAL,
                                             DEFAULT_DESIRED_SLAVE_LATENCY,
                                             DEFAULT_DESIRED_CONN_TIMEOUT,
                                             hidEmuTaskId);

        return (events ^ HID_START_PARAM_UPDATE_EVT);
    }

    if (events & HID_START_PHY_UPDATE_EVT)
    {
        // start phy update
        APP_DBG("Send Phy Update %X...", GAPRole_UpdatePHY(hidEmuConnHandle, 0, GAP_PHY_BIT_LE_2M,
                                                             GAP_PHY_BIT_LE_2M, 0));

        return (events ^ HID_START_PHY_UPDATE_EVT);
    }

    if (events & HID_START_REPORT_EVT)
    {
        hidSendMessage();
        tmos_start_task(hidEmuTaskId, HID_START_REPORT_EVT, 200);
        return (events ^ HID_START_REPORT_EVT);
    }
    return 0;
}

static uint8_t asciiToKeycode(char character)
{
    switch (character) {
        case 'a':
        case 'A':
            return KEY_A;
        case 'b':
        case 'B':
            return KEY_B;
        case 'c':
        case 'C':
            return KEY_C;
        case 'd':
        case 'D':
            return KEY_D;
        case 'e':
        case 'E':
            return KEY_E;
        case 'f':
        case 'F':
            return KEY_F;
        case 'g':
        case 'G':
            return KEY_G;
        case 'h':
        case 'H':
            return KEY_H;
        case 'i':
        case 'I':
            return KEY_I;
        case 'j':
        case 'J':
            return KEY_J;
        case 'k':
        case 'K':
            return KEY_K;
        case 'l':
        case 'L':
            return KEY_L;
        case 'm':
        case 'M':
            return KEY_M;
        case 'n':
        case 'N':
            return KEY_N;
        case 'o':
        case 'O':
            return KEY_O;
        case 'p':
        case 'P':
            return KEY_P;
        case 'q':
        case 'Q':
            return KEY_Q;
        case 'r':
        case 'R':
            return KEY_R;
        case 's':
        case 'S':
            return KEY_S;
        case 't':
        case 'T':
            return KEY_T;
        case 'u':
        case 'U':
            return KEY_U;
        case 'v':
        case 'V':
            return KEY_V;
        case 'w':
        case 'W':
            return KEY_W;
        case 'x':
        case 'X':
            return KEY_X;
        case 'y':
        case 'Y':
            return KEY_Y;
        case 'z':
        case 'Z':
            return KEY_Z;
        case '1':
            return KEY_1;
        case '2':
            return KEY_2;
        case '3':
            return KEY_3;
        case '4':
            return KEY_4;
        case '5':
            return KEY_5;
        case '6':
            return KEY_6;
        case '7':
            return KEY_7;
        case '8':
            return KEY_8;
        case '9':
            return KEY_9;
        case '0':
            return KEY_0;
        case '\n':
            return KEY_ENTER;
        case '\b':
            return KEY_BACKSPACE;
        case '\t':
            return KEY_TAB;
        case ' ':
            return KEY_SPACE;
        case '-':
        case '_':
            return KEY_MINUS;
        case '=':
        case '+':
            return KEY_EQUAL;
        case '[':
        case '{':
            return KEY_LEFTBRACE;
        case ']':
        case '}':
            return KEY_RIGHTBRACE;
        case '\\':
        case '|':
            return KEY_BACKSLASH;
        case '#':
            return KEY_HASHTILDE;
        case ';':
        case ':':
            return KEY_SEMICOLON;
        case '\'':
        case '\"':
            return KEY_APOSTROPHE;
        case '`':
        case '~':
            return KEY_GRAVE;
        case ',':
            return KEY_COMMA;
        case '.':
        case '>':
            return KEY_DOT;
        case '/':
            return KEY_SLASH;
        default:
            return KEY_NONE;
    }
}

static void hidSendMessage()
{
    static uint8_t index = 0;
    char message[] = "Coloca todos los LEDs en verde. ";
    uint8_t messageSize = sizeof(message) / sizeof(message[0]);
    char character = message[index];
    index++;
    if (index == messageSize)
        index = 0;

    // APP_DBG("Sending: %c", character);
    hidEmuSendKbdReport(asciiToKeycode(character));
    hidEmuSendKbdReport(KEY_NONE);
}

/*********************************************************************
 * @fn      hidEmuSendKbdReport
 *
 * @brief   Build and send a HID keyboard report.
 *
 * @param   keycode - HID keycode.
 *
 * @return  none
 */
static void hidEmuSendKbdReport(uint8_t keycode)
{
    uint8_t buf[HID_KEYBOARD_IN_RPT_LEN];

    buf[0] = 0;       // Modifier keys
    buf[1] = 0;       // Reserved
    buf[2] = keycode; // Keycode 1
    buf[3] = 0;       // Keycode 2
    buf[4] = 0;       // Keycode 3
    buf[5] = 0;       // Keycode 4
    buf[6] = 0;       // Keycode 5
    buf[7] = 0;       // Keycode 6

    HidDev_Report(HID_RPT_ID_KEY_IN, HID_REPORT_TYPE_INPUT,
                  HID_KEYBOARD_IN_RPT_LEN, buf);
}

/*********************************************************************
 * @fn      hidEmu_ProcessTMOSMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void hidEmu_ProcessTMOSMsg(tmos_event_hdr_t *pMsg)
{
    switch (pMsg->event)
    {
    default:
        break;
    }
}

/*********************************************************************
 * @fn      hidEmuStateCB
 *
 * @brief   GAP state change callback.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void hidEmuStateCB(gapRole_States_t newState, gapRoleEvent_t *pEvent)
{
    switch (newState & GAPROLE_STATE_ADV_MASK)
    {
    case GAPROLE_STARTED:
    {
        uint8_t ownAddr[6];
        GAPRole_GetParameter(GAPROLE_BD_ADDR, ownAddr);
        GAP_ConfigDeviceAddr(ADDRTYPE_STATIC, ownAddr);
        APP_DBG("Initialized...");
    }
    break;

    case GAPROLE_ADVERTISING:
        if (pEvent->gap.opcode == GAP_MAKE_DISCOVERABLE_DONE_EVENT)
        {
            APP_DBG("Advertising...");
        }
        break;

    case GAPROLE_CONNECTED:
        if (pEvent->gap.opcode == GAP_LINK_ESTABLISHED_EVENT)
        {
            gapEstLinkReqEvent_t *event = (gapEstLinkReqEvent_t *)pEvent;

            // get connection handle
            hidEmuConnHandle = event->connectionHandle;
            tmos_start_task(hidEmuTaskId, HID_START_PARAM_UPDATE_EVT, HID_START_PARAM_UPDATE_EVT_DELAY);
            APP_DBG("Connected...");
        }
        break;

    case GAPROLE_CONNECTED_ADV:
        if (pEvent->gap.opcode == GAP_MAKE_DISCOVERABLE_DONE_EVENT)
        {
            APP_DBG("Connected Advertising..");
        }
        break;

    case GAPROLE_WAITING:
        if (pEvent->gap.opcode == GAP_END_DISCOVERABLE_DONE_EVENT)
        {
            APP_DBG("Waiting for advertising..");
        }
        else if (pEvent->gap.opcode == GAP_LINK_TERMINATED_EVENT)
        {
            APP_DBG("Disconnected.. Reason:%x", pEvent->linkTerminate.reason);
        }
        else if (pEvent->gap.opcode == GAP_LINK_ESTABLISHED_EVENT)
        {
            APP_DBG("Advertising timeout..");
        }
        // Enable advertising
        {
            uint8_t initial_advertising_enable = TRUE;
            // Set the GAP Role Parameters
            GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &initial_advertising_enable);
        }
        break;

    case GAPROLE_ERROR:
        APP_DBG("Error %x ..", pEvent->gap.opcode);
        break;

    default:
        break;
    }
}

/*********************************************************************
 * @fn      hidEmuRcvReport
 *
 * @brief   Process an incoming HID keyboard report.
 *
 * @param   len - Length of report.
 * @param   pData - Report data.
 *
 * @return  status
 */
static uint8_t hidEmuRcvReport(uint8_t len, uint8_t *pData)
{
    // verify data length
    if (len == HID_LED_OUT_RPT_LEN)
    {
        // set LEDs
        return SUCCESS;
    }
    else
    {
        return ATT_ERR_INVALID_VALUE_SIZE;
    }
}

/*********************************************************************
 * @fn      hidEmuRptCB
 *
 * @brief   HID Dev report callback.
 *
 * @param   id - HID report ID.
 * @param   type - HID report type.
 * @param   uuid - attribute uuid.
 * @param   oper - operation:  read, write, etc.
 * @param   len - Length of report.
 * @param   pData - Report data.
 *
 * @return  GATT status code.
 */
static uint8_t hidEmuRptCB(uint8_t id, uint8_t type, uint16_t uuid,
                           uint8_t oper, uint16_t *pLen, uint8_t *pData)
{
    uint8_t status = SUCCESS;

    // write
    if (oper == HID_DEV_OPER_WRITE)
    {
        if (uuid == REPORT_UUID)
        {
            // process write to LED output report; ignore others
            if (type == HID_REPORT_TYPE_OUTPUT)
            {
                status = hidEmuRcvReport(*pLen, pData);
            }
        }

        if (status == SUCCESS)
        {
            status = Hid_SetParameter(id, type, uuid, *pLen, pData);
        }
    }
    // read
    else if (oper == HID_DEV_OPER_READ)
    {
        status = Hid_GetParameter(id, type, uuid, pLen, pData);
    }
    // notifications enabled
    else if (oper == HID_DEV_OPER_ENABLE)
    {
        tmos_start_task(hidEmuTaskId, HID_START_REPORT_EVT, 500);
    }
    return status;
}

/*********************************************************************
 * @fn      hidEmuEvtCB
 *
 * @brief   HID Dev event callback.
 *
 * @param   evt - event ID.
 *
 * @return  HID response code.
 */
static void hidEmuEvtCB(uint8_t evt)
{
    // process enter/exit suspend or enter/exit boot mode
    return;
}

/*********************************************************************
 * @fn      simpleProfileChangeCB
 *
 * @brief   Callback from SimpleBLEProfile indicating a value change
 *
 * @param   paramID - parameter ID of the value that was changed.
 *          pValue - pointer to data that was changed
 *          len - length of data
 *
 * @return  none
 */
static void simpleProfileChangeCB(uint8_t paramID, uint8_t *pValue, uint16_t len)
{
    switch (paramID)
    {
    case SIMPLEPROFILE_CHAR1:
    {
        uint8_t newValue[SIMPLEPROFILE_CHAR1_LEN];
        tmos_memcpy(newValue, pValue, len);
        APP_DBG("profile ChangeCB CHAR1.. ");
        App_peripheral_reveived(newValue, len);
        break;
    }

    case SIMPLEPROFILE_CHAR3:
    {
        uint8_t newValue[SIMPLEPROFILE_CHAR3_LEN];
        tmos_memcpy(newValue, pValue, len);
        APP_DBG("profile ChangeCB CHAR3..");
        break;
    }

    default:
        // should not reach here!
        break;
    }
}

/*********************************************************************
*********************************************************************/
