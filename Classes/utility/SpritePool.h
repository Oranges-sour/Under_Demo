#pragma once
#ifndef __SPRITE_POOL_H__
#define __SPRITE_POOL_H__

#include <vector>

#include "game/game_object/GameObject.h"
using namespace std;

class SpritePool {
public:
    // static void init(int max_size);
    // static GameObject* getSprite();
    // static void saveBack(GameObject* game_object);

private:
    static vector<GameObject*> _pool;
    static GameObject* first_available;
    static int max_size;
};

#endif  // !__SPRITE_POOL_H__
