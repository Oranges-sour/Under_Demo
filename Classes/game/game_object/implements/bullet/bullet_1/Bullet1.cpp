#include "Bullet1.h"

#include "Bullet1AI.h"
#include "Bullet1Physics.h"
#include "game/game_object/GameObject.h"
#include "game/game_world/GameWorld.h"
#include "utility/GameObjectInfo.h"
#include "utility/PhysicsShapeCache.h"

GameObject* Bullet1::create(GameWorld* world, const string& json_key,
                            const Vec2& start_pos, const Vec2& move_vec) {
    auto& info = GameObjectInfo::instance()->get(json_key);
    string sprite_frame = info["sprite_frame"];
    string physics_shape = info["physics_shape"];
    float move_speed = info["move_speed"];
    float rotate_speed = info["rotate_speed"];
    int dead_particle_cnt = info["dead_particle_cnt"];
    string dead_particle_name = info["dead_particle_name"];
    vector<json> lights = info["world_lights"];

    auto ob = world->newObject(layer_bullet, start_pos);
    ob->initWithSpriteFrameName(sprite_frame);
    ob->setGameObjectType(object_type_bullet);
    PhysicsShapeCache::getInstance()->setBodyOnSprite(physics_shape, ob);

    auto ai = make_shared<Bullet1AI>(move_vec, move_speed, rotate_speed);
    auto phy = make_shared<Bullet1Physics>(start_pos, dead_particle_cnt,
                                           dead_particle_name);

    ob->addGameComponent(ai);
    ob->addGameComponent(phy);

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
