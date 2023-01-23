#include "Player1.h"

#include "Player1AI.h"
#include "Player1Physics.h"
#include "game/game_object/GameObject.h"
#include "game/game_world/GameWorld.h"
#include "utility/GameObjectInfo.h"
#include "utility/PhysicsShapeCache.h"
#include "utility/json/json.h"

GameObject* Player1::create(GameWorld* world, const json& json_key,
                            const Vec2& start_pos, const string& uid) {
    auto& info = GameObjectInfo::instance()->get(json_key);
    string sprite_frame = info["sprite_frame"];
    string physics_shape = info["physics_shape"];
    float move_speed = info["move_speed"];
    float jump_speed = info["jump_speed"];
    float wall_contact_left_top_x = info["wall_contact_left_top_x"];
    float wall_contact_left_top_y = info["wall_contact_left_top_y"];
    float wall_contact_right_bottom_x = info["wall_contact_right_bottom_x"];
    float wall_contact_right_bottom_y = info["wall_contact_right_bottom_y"];
    float gravity = info["gravity"];
    vector<string> frame_action_stay = info["frame_action_stay"];
    vector<string> frame_action_run = info["frame_action_run"];
    vector<string> frame_action_jump = info["frame_action_jump"];
    vector<string> frame_action_jump_stay = info["frame_action_jump_stay"];
    vector<json> lights = info["world_lights"];

    auto ob = world->newObject(layer_player, start_pos);
    ob->initWithSpriteFrameName(sprite_frame);
    PhysicsShapeCache::getInstance()->setBodyOnSprite(physics_shape, ob);
    ob->setGameObjectType(object_type_player);
    // Íæ¼ÒÓÐÌØÊâuid
    ob->setUID(uid);

    auto ai = make_shared<Player1AI>();
    auto phy = make_shared<Player1Physics>(
        start_pos, move_speed, jump_speed, frame_action_stay, frame_action_run,
        frame_action_jump, frame_action_jump_stay);

    auto speed_comp = make_shared<PhysicsComponent::SpeedComponent>();
    auto gravity_comp =
        make_shared<PhysicsComponent::GravityComponent>(gravity, speed_comp);
    auto wall_contact_comp =
        make_shared<PhysicsComponent::WallContactComponent>(
            Vec2(wall_contact_left_top_x, wall_contact_left_top_y),
            Vec2(wall_contact_right_bottom_x, wall_contact_right_bottom_y),
            speed_comp);

    phy->setGravityComponent(gravity_comp);
    phy->setSpeedComponent(speed_comp);
    phy->setWallContactComponent(wall_contact_comp);

    ob->addGameComponent(phy);
    ob->addGameComponent(ai);

    for (auto& it : lights) {
        string key = it["key"];
        int type = it["type"];
        int r = it["r"];
        int g = it["g"];
        int b = it["b"];
        float radius = it["radius"];
        float opacity = it["opacity"];
        WorldLight light(Color3B((GLubyte)r, (GLubyte)g, (GLubyte)b), radius,
                         opacity, (WorldLight::WorldLightType)type);

        ob->addWorldLight(light, key);
    }

    return ob;
}
