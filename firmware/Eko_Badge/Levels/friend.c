#include "friend.h"

BOOL enableFriendSearch;
uint16_t friendsCounter;
friend_t friends[FRIENDS_MAX];

void Friends_Init()
{
    enableFriendSearch = FALSE;
    // friendsCounter = 0;
    friendsCounter = Flash_Get_Friends_Counter();
}

void Friends_List()
{
    for (uint16_t i = 0; i < friendsCounter; i++)
    {
        APP_DBG("Friend %d: %02X:%02X:%02X:%02X:%02X:%02X", i,
                friends[i].address[5], friends[i].address[4],
                friends[i].address[3], friends[i].address[2],
                friends[i].address[1], friends[i].address[0]);
    }
}