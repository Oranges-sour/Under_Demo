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
    SC float gravity = info["gravity"];
    SC float wall_contact_left_bottom_x = info["wall_contact_left_bottom_x"];
    SC float wall_contact_left_bottom_y = info["wall_contact_left_bottom_y"];
    SC float wall_contact_right_top_x = info["wall_contact_right_top_x"];
    SC float wall_contact_right_top_y = info["wall_contact_right_top_y"];
    SC vector<json> lights = info["world_lights"];

    auto ob = world->newObject(layer_enemy, start_pos);
    ob->initWithSpriteFrameName(sprite_frame);
    ob->setGameObjectType(object_type_enemy);
    PhysicsShapeCache::getInstance()->setBodyOnSprite(physics_shape, ob);
    WorldLight::setWorldLight(lights, ob);

    auto ai =
        make_shared<Enemy1AI>(detect_range, bullet_json_key, attack_speed);
    auto phy = make_shared<Enemy1Physics>(start_pos);

    auto speed_comp = make_shared<PhysicsComponent::SpeedComponent>();
    auto gravity_comp =
        make_shared<PhysicsComponent::GravityComponent>(gravity, speed_comp);
    auto wall_contact_comp =
        make_shared<PhysicsComponent::WallContactComponent>(
            Vec2(wall_contact_left_bottom_x, wall_contact_left_bottom_y),
            Vec2(wall_contact_right_top_x, wall_contact_right_top_y),
            speed_comp);

    phy->setGravityComponent(gravity_comp);
    phy->setSpeedComponent(speed_comp);
    phy->setWallContactComponent(wall_contact_comp);

    ob->addGameComponent(ai);
    ob->addGameComponent(phy);

    return ob;
}

#undef SC