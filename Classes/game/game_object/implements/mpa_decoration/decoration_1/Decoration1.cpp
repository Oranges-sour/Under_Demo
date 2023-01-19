#include "Decoration1.h"

#include "Decoration1AI.h"
#include "game/game_object/GameObject.h"
#include "game/game_world/GameWorld.h"

GameObject* MapDecoration1::create(GameWorld* world, const Vec2& start_pos) {
    auto ob = world->newObject(layer_map_decoration, start_pos);
    ob->setGameObjectType(object_type_decoration);
    ob->setAnchorPoint(Vec2(0, 0));
    ob->initWithSpriteFrameName("game_map_decoration_1.png");

    auto ai = make_shared<MapDecoration1AI>();

    ob->addGameComponent(ai);

    WorldLight light(Color3B(150, 225, 238), 240, 1.0,
                     WorldLight::world_light_type3, Vec2(0, -22));

    ob->addWorldLight(light, "main_light");

    return ob;
}
