#ifndef __PLAYER_1_H__
#define __PLAYER_1_H__

#include "game/game_object/GameObject.h"
#include "game/game_world/GameWorld.h"

class Player1 {
public:
    static GameObject* create(GameWorld* world, const Vec2& start_pos,
                              const string& uid);
};

#endif