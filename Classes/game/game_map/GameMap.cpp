#include "GameMap.h"

#include "game/game_object/GameObject.h"
#include "game/game_world/GameWorld.h"
#include "utility/PhysicsShapeCache.h"

void GameMap::init(shared_ptr<MapGeneratorComponent> mapGenerator,
                   shared_ptr<MapHelperComponent> mapHelper,
                   shared_ptr<MapPhysicsComponent> mapPhysics) {
    this->_map_helper = mapHelper;
    this->_map_physics = mapPhysics;

    mapGenerator->generate(w, h, this->map);
}

void GameMap::updateLogic(GameWorld* game_world) {
    if (_map_physics) {
        _map_physics->updateLogic(game_world);
    }
}

/////////////////////////////////////////////////////////////////////////

void MapGameObjectTileComponent::updateLogic(GameObject* ob) {
    cnt -= 1;
    if (cnt == 0) {
        _dirty->erase(uid);
        ob->removeFromParent();
    }
}

void MapGameObjectTileComponent::receiveEvent(GameObject* ob,
                                              const json& event) {
    string type = event["type"];
    if (type == "refresh") {
        cnt = 10;
    }
}
