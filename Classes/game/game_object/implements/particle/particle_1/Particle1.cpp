#include "Particle1.h"

#include "Particle1AI.h"
#include "Particle1Physics.h"
#include "game/game_object/GameObject.h"
#include "game/game_world/GameWorld.h"
#include "utility/GameObjectInfo.h"
#include "utility/math/MyMath.h"

GameObject* Particle1::create(GameWorld* world, const string& json_key,
                              const Vec2& start_pos) {
    auto& info = GameObjectInfo::instance()->get(json_key);

    string sprite_frame = info["sprite_frame"];
    int live_frame = info["live_frame"];
    float move_speed = info["move_speed"];
    float rotate_speed = info["rotate_speed"];
    float light_decrease_rate = info["light_decrease_rate"];
    float scale_decrease_rate = info["scale_decrease_rate"];
    float opacity_decrease_rate = info["opacity_decrease_rate"];
    float direction_angle_min = info["direction_angle_min"];
    float direction_angle_max = info["direction_angle_max"];
    vector<json> lights = info["world_lights"];

    auto ob = world->newObject(layer_particle, start_pos);
    ob->initWithSpriteFrameName(sprite_frame);
    ob->setGameObjectType(object_type_particle);
    WorldLight::setWorldLight(lights, ob);

    rand_float ran(direction_angle_min, direction_angle_max);
    float a = ran(*world->getGlobalRandom());

    auto ai =
        make_shared<Particle1AI>(Vec2(cos(a), sin(a)), live_frame, move_speed,
                                 rotate_speed, light_decrease_rate);
    auto phy = make_shared<Particle1Physics>(start_pos, scale_decrease_rate,
                                             opacity_decrease_rate);

    ob->addGameComponent(ai);
    ob->addGameComponent(phy);

    return ob;
}
