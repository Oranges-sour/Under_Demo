#include "Decoration2.h"

#include "Decoration2AI.h"
#include "game/game_object/GameObject.h"
#include "game/game_world/GameWorld.h"

GameObject* MapDecoration2::create(GameWorld* world, const Vec2& start_pos) {
    auto ob = world->newObject(layer_map_decoration, start_pos);
    ob->setGameObjectType(object_type_decoration);
    ob->setAnchorPoint(Vec2(0, 0));

    auto ai = make_shared<MapDecoration2AI>();

    ob->addGameComponent(ai);

    WorldLight light(Color3B(222, 191, 238), 280, 1.0,
                     WorldLight::world_light_type2, Vec2(0, 0));

    ob->addWorldLight(light, "main_light");

    return ob;
}
