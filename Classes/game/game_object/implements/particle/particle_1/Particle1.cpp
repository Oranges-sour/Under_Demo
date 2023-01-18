#include "Particle1.h"

#include "Particle1AI.h"
#include "Particle1Physics.h"

#include "game/game_object/GameObject.h"
#include "game/game_world/GameWorld.h"

GameObject* Particle1::create(GameWorld* world, const Vec2& start_pos) {
    auto ob = world->newObject(layer_particle, start_pos);
    ob->initWithSpriteFrameName("enemy_bullet_1_par.png");
    ob->setGameObjectType(object_type_particle);

    rand_float r(0, 3.14 * 2);
    float a = r(*world->getGlobalRandom());

    auto ai = make_shared<Particle1AI>(cos(a), sin(a));
    auto phy = make_shared<Particle1Physics>(start_pos);

    ob->addGameComponent(ai);
    ob->addGameComponent(phy);

    WorldLight light(Color3B(194, 120, 194), 140, 1.0,
                     WorldLight::world_light_type2);

    ob->addWorldLight(light, "main_light");

    return ob;
}
