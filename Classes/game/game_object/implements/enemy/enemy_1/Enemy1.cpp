#include "Enemy1.h"

#include "Enemy1AI.h"
#include "Enemy1Physics.h"
#include "game/game_object/GameObject.h"
#include "game/game_world/GameWorld.h"
#include "utility/GameObjectInfo.h"
#include "utility/PhysicsShapeCache.h"

#define SC static const

GameObject* Enemy1::create(GameWorld* world, const json& json_key,
                           const Vec2& start_pos) {
    auto& info = GameObjectInfo::instance()->get(json_key);
    SC string sprite_frame = info["sprite_frame"];
    SC string physics_shape = info["physics_shape"];
    SC string bullet_json_key = info["bullet_json_key"];
    SC int attack_speed = info["attack_speed"];
    SC float detect_range = info["detect_range"];
    SC vector<json> lights = info["world_lights"];

    auto ob = world->newObject(layer_enemy, start_pos);
    ob->initWithSpriteFrameName(sprite_frame);
    ob->setGameObjectType(object_type_enemy);
    PhysicsShapeCache::getInstance()->setBodyOnSprite(physics_shape, ob);
    WorldLight::setWorldLight(lights, ob);

    auto ai =
        make_shared<Enemy1AI>(detect_range, bullet_json_key, attack_speed);
    auto phy = make_shared<Enemy1Physics>();

    ob->addGameComponent(ai);
    ob->addGameComponent(phy);

    return ob;
}

#undef SC