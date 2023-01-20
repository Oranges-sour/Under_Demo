#ifndef __MAP_PHYSICS_1_H__
#define __MAP_PHYSICS_1_H__

#include "game/game_map/GameMap.h"

class MapPhysics1 : public MapPhysicsComponent {
public:
    virtual void updateLogic(GameWorld* game_world);

private:
    map<string, GameObject*> _dirty;
};

#endif