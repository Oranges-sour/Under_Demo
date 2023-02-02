#include "Bullet2.h"

#include "Bullet2AI.h"
#include "Bullet2Physics.h"
#include "game/game_object/GameObject.h"
#include "game/game_world/GameWorld.h"
#include "utility/GameObjectInfo.h"
#include "utility/PhysicsShapeCache.h"

#define SC static const

GameObject* Bullet2::create(GameWorld* world, const string& json_key,
                            const Vec2& start_pos, const Vec2& end_pos) {
    auto& info = GameObjectInfo::instance()->get(json_key);
    SC string sprite_frame = info["sprite_frame"];
    SC string physics_shape = info["physics_shape"];
    SC float move_speed = info["move_speed"];
    SC string particle_move_json_key = info["particle_move_json_key"];
    SC float particle_move_cnt_per_frame = info["particle_move_cnt_per_frame"];
    SC string particle_explode_json_key = info["particle_explode_json_key"];
    SC int particle_explode_cnt = info["particle_explode_cnt"];
    SC vector<json> lights = info["world_lights"];

    auto ob = world->newObject(layer_bullet, start_pos);
    ob->initWithSpriteFrameName(sprite_frame);
    ob->setGameObjectType(object_type_bullet);
    PhysicsShapeCache::getInstance()->setBodyOnSprite(physics_shape, ob);
    WorldLight::setWorldLight(lights, ob);

    auto ai = make_shared<Bullet2AI>(start_pos, end_pos, particle_move_json_key,
                                     particle_move_cnt_per_frame);
    auto phy = make_shared<Bullet2Physics>(start_pos, end_pos, move_speed,
                                           particle_explode_cnt,
                                           particle_explode_json_key);

    ob->addGameComponent(ai);
    ob->addGameComponent(phy);

    return ob;
}
