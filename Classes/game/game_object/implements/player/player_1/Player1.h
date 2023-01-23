#ifndef __PLAYER_1_H__
#define __PLAYER_1_H__

#include "cocos2d.h"
USING_NS_CC;

#include <string>
using namespace std;

#include "utility/json/json.h"

class GameWorld;
class GameObject;

class Player1 {
public:
    static GameObject* create(GameWorld* world, const json& json_key,
                              const Vec2& start_pos, const string& uid);
};

#endif