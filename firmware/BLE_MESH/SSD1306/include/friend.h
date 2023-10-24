#ifndef FRIEND_H
#define FRIEND_H

#include "HAL.h"
#include "app_mesh_config.h"

#define FRIENDS_MAX 500

typedef struct {
    uint8_t address[6];
} friend_t;

extern BOOL enableFriendSearch;
extern uint16_t friendsCounter;
extern friend_t friends[FRIENDS_MAX];

void Friends_Init();
void Friends_List();

#endif