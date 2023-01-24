#ifndef __START_POINT_H__
#define __START_POINT_H__

#include "cocos2d.h"
USING_NS_CC;

#include <string>
using namespace std;

class GameWorld;
class GameObject;

class StartPoint {
public:
    static GameObject* create(GameWorld* world, const string& json_key,
                              const Vec2& pos);
};

#endif