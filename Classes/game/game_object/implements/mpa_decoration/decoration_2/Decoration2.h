#ifndef __MAP_DECORATION_2_H__
#define __MAP_DECORATION_2_H__

#include "cocos2d.h"
USING_NS_CC;

class GameWorld;
class GameObject;

class MapDecoration2 {
public:
    static GameObject* create(GameWorld* world, const Vec2& start_pos);
};

#endif