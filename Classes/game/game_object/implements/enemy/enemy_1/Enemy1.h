#ifndef __ENEMY_1_H__
#define __ENEMY_1_H__

#include "cocos2d.h"
USING_NS_CC;

#include "utility/json/json.h"

class GameObject;
class GameWorld;

class Enemy1 {
public:
    static GameObject* create(GameWorld* world, const json& json_key,
                              const Vec2& start_pos);
};

#endif