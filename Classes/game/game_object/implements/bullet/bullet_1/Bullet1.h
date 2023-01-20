#ifndef __BULLET_1_H__
#define __BULLET_1_H__

#include "cocos2d.h"
USING_NS_CC;

#include <string>
using namespace std;

class GameWorld;
class GameObject;

class Bullet1 {
public:
    static GameObject* create(GameWorld* world, const string& json_key,
                              const Vec2& start_pos, const Vec2& move_vec);
};

#endif