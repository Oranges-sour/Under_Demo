#include "Player1.h"

#include "Player1AI.h"
#include "Player1Physics.h"
#include "game/game_object/GameObject.h"
#include "game/game_world/GameWorld.h"
#include "utility/GameObjectInfo.h"
#include "utility/PhysicsShapeCache.h"
#include "utility/json/json.h"

#define SC static const

GameObject* Player1::create(GameWorld* world, const json& json_key,
                            const Vec2& start_pos, const string& uid) {
    SC auto& info = GameObjectInfo::instance()->get(json_key);
    SC string sprite_frame = info["sprite_frame"];
    SC string physics_shape = info["physics_shape"];
    SC float move_speed = info["move_speed"];
    SC float jump_speed = info["jump_speed"];
    SC float wall_contact_left_bottom_x = info["wall_contact_left_bottom_x"];
    SC float wall_contact_left_bottom_y = info["wall_contact_left_bottom_y"];
    SC float wall_contact_right_top_x = info["wall_contact_right_top_x"];
    SC float wall_contact_right_top_y = info["wall_contact_right_top_y"];
    SC float gravity = info["gravity"];
    SC vector<string> frame_action_stay = info["frame_action_stay"];
    SC vector<string> frame_action_run = info["frame_action_run"];
    SC vector<string> frame_action_jump = info["frame_action_jump"];
    SC vector<string> frame_action_jump_stay = info["frame_action_jump_stay"];
    SC vector<string> frame_action_attack_near =
        info["frame_action_attack_near"];
    SC vector<json> lights = info["world_lights"];

    auto ob = world->newObject(layer_player, start_pos);
    ob->initWithSpriteFrameName(sprite_frame);
    PhysicsShapeCache::getInstance()->setBodyOnSprite(physics_shape, ob);
    WorldLight::setWorldLight(lights, ob);

    ob->setGameObjectType(object_type_player);
    // Íæ¼ÒÓÐÌØÊâuid
    ob->setUID(uid);

    auto ai = make_shared<Player1AI>();
    auto phy = make_shared<Player1Physics>(
        start_pos, move_speed, jump_speed, frame_action_stay, frame_action_run,
        frame_action_jump, frame_action_jump_stay, frame_action_attack_near);

    auto speed_comp = make_shared<Player1Speed>();
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

    ob->addGameComponent(phy);
    ob->addGameComponent(ai);

    return ob;
}

#undef SC
