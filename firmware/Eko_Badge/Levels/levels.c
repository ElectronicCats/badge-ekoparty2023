#include "levels.h"

static BOOL receiveData;
static uint32_t foundationYear;
static uint8_t level;

void Levels_Init()
{
    Friends_Init();
    foundationYear = 0;
    Disable_Receive_Data();
    Set_Level(2);
}

uint8_t Get_Level()
{
    return level;
}

void Set_Level(uint8_t newLevel)
{
    level = newLevel;
}

void Enable_Receive_Data()
{
    receiveData = TRUE;
}

void Disable_Receive_Data()
{
    receiveData = FALSE;
}

BOOL Is_Receive_Data_Enabled()
{
    return receiveData;
}

uint32_t Get_Foundation_Year()
{
    return foundationYear;
}

void Set_Foundation_Year(uint32_t year)
{
    foundationYear = year;
}
