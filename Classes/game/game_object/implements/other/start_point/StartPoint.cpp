#include "StartPoint.h"

#include "StartPointAI.h"
#include "game/game_object/GameObject.h"
#include "game/game_world/GameWorld.h"
#include "utility/GameObjectInfo.h"

GameObject* StartPoint::create(GameWorld* world, const string& json_key,
                               const Vec2& pos) {
    auto& info = GameObjectInfo::instance()->get("start_point");
    string sprite_frame = info["sprite_frame"];
    float particle_cnt_per_frame = info["particle_cnt_per_frame"];
    string particle_name = info["particle_name"];
    float offset_1_x = info["offset_1_x"];
    float offset_1_y = info["offset_1_y"];
    float offset_2_x = info["offset_2_x"];
    float offset_2_y = info["offset_2_y"];
    vector<json> lights = info["world_lights"];

    auto ob = world->newObject(layer_map_decoration, pos);
    ob->initWithSpriteFrameName(sprite_frame);

    ob->setAnchorPoint(Vec2(0, 0));
    ob->setPosition(pos);

    auto ai = make_shared<StartPointAI>(particle_cnt_per_frame, particle_name,
                                        Vec2(offset_1_x, offset_1_y),
                                        Vec2(offset_2_x, offset_2_y));
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
        light.offset = Vec2(96, 0);
        ob->addWorldLight(light, key);
    }

    return ob;
}
