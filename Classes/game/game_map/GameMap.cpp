#include "GameMap.h"

#include "game/game_object/GameObject.h"
#include "game/game_world/GameWorld.h"
#include "utility/PhysicsShapeCache.h"

void GameMap::init(shared_ptr<MapGeneratorComponent> mapGenerator,
                   shared_ptr<MapHelperComponent> mapHelper,
                   shared_ptr<MapPhysicsComponent> mapPhysics) {
    this->_map_helper = mapHelper;
    this->_map_physics = mapPhysics;

    mapGenerator->generate(_w, _h, this->_map);
}

void GameMap::update(GameWorld* game_world) {
    if (_map_physics) {
        _map_physics->updateLogic(game_world);
    }
}

/////////////////////////////////////////////////////////////////////////
MapGameObjectTileComponent::MapGameObjectTileComponent() {
    this->schedule([&](GameObject* ob) { upd(ob); }, 0, "update");
}

void MapGameObjectTileComponent::receiveEvent(GameObject* ob,
                                              const GameEvent& event) {
    //TODO : 
    /*string type = event["type"];
    if (type == "refresh") {
        _cnt = 10;
    }*/
}

void MapGameObjectTileComponent::upd(GameObject* ob) {
    _cnt -= 1;
    if (_cnt == 0) {
        _dirty->erase(_uid);
        ob->removeFromParent();
    }
}
