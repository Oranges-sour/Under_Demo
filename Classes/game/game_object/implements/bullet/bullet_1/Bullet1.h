#ifndef __BULLET_1_H__
#define __BULLET_1_H__

#include "game/game_object/GameObject.h"
#include "game/game_world/GameWorld.h"

class Bullet1 {
public:
    static GameObject* create(GameWorld* world, const Vec2& start_pos,
                              const Vec2& move_vec);
};

#endif