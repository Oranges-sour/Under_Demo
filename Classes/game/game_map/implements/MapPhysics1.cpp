#include "MapPhysics1.h"

#include "utility/PhysicsShapeCache.h"

void MapPhysicsComponent1::updateLogic(GameWorld* game_world) {
    const int dx[8] = {-1, 0, 1, 1, 1, 0, -1, -1};
    const int dy[8] = {1, 1, 1, 0, -1, -1, -1, 0};

    auto _map = game_world->getGameMap();

    const auto inside = [&](int x, int y) {
        if (x >= 1 && x <= _map->get().w && y >= 1 && y <= _map->get().h) {
            return true;
        }
        return false;
    };

    const auto make_uid = [&](const iVec2& pos) {
        stringstream ss;
        if (pos.x < 10) {
            ss << "00" << pos.x;
        } else if (pos.x < 100) {
            ss << "0" << pos.x;
        } else {
            ss << pos.x;
        }
        if (pos.y < 10) {
            ss << "00" << pos.y;
        } else if (pos.y < 100) {
            ss << "0" << pos.y;
        } else {
            ss << pos.y;
        }
        return string(ss.str());
    };

    auto& quad = game_world->get_objects();
    quad.visit_in_rect(
        {-1, 500}, {500, -1}, [&](const iVec2& cor, GameObject* ob) {
            auto type = ob->getGameObjectType();
            if (type != object_type_player && type != object_type_bullet &&
                type != object_type_enemy && type != object_type_equipment) {
                return;
            }

            auto box = ob->getBoundingBox();
            auto left_bottom = _map->getMapHelper()->convert_in_map(box.origin);
            auto right_top =
                _map->getMapHelper()->convert_in_map(box.origin + box.size);

            for (int xx = left_bottom.x; xx <= right_top.x; ++xx) {
                for (int yy = left_bottom.y; yy <= right_top.y; ++yy) {
                    auto map_type = _map->get()[xx][yy];
                    if (map_type == air) {
                        continue;
                    }
                    //
                    auto uid = make_uid({xx, yy});
                    auto iter = _dirty.find(uid);
                    if (iter != _dirty.end()) {
                        json event;
                        event["type"] = "refresh";
                        iter->second->pushEvent(event);
                    } else {
                        auto sp = game_world->newObject(
                            4, Vec2((xx - 1) * 64 + 5,
                                    (yy - 1) * 64 + 5) /*保证落在格子里*/);
                        sp->initWithSpriteFrameName(
                            "game_map_tile_physics.png");
                        sp->setVisible(false);

                        sp->setGameObjectType(object_type_wall);
                        sp->setAnchorPoint(Vec2(0, 0));
                        sp->setPosition(Vec2((xx - 1) * 64, (yy - 1) * 64));

                        auto comp = make_shared<MapGameObjectTileComponent>();
                        comp->init(&_dirty, uid);
                        sp->addGameComponent(comp);

                        PhysicsShapeCache::getInstance()->setBodyOnSprite(
                            "game_map_tile_physics", sp);

                        _dirty.insert({uid, sp});
                    }
                }
            }
        });
}