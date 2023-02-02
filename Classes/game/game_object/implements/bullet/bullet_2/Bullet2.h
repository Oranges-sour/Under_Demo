#ifndef __BULLET_2_H__
#define __BULLET_2_H__

#include "cocos2d.h"
USING_NS_CC;

#include <string>
using namespace std;

class GameWorld;
class GameObject;

class Bullet2 {
public:
    static GameObject* create(GameWorld* world, const string& json_key,
                              const Vec2& start_pos, const Vec2& end_pos);
};

#endif