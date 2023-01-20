#ifndef __PARTICLE_1_H__
#define __PARTICLE_1_H__

#include "cocos2d.h"
USING_NS_CC;

#include <string>
using namespace std;

class GameObject;
class GameWorld;

class Particle1 {
public:
    static GameObject* create(GameWorld* world, const string& json_key,
                              const Vec2& start_pos);
};

#endif