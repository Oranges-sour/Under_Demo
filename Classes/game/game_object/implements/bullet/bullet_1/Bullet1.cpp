#include "Bullet1.h"

#include "Bullet1AI.h"
#include "Bullet1Physics.h"
#include "utility/PhysicsShapeCache.h"

GameObject* Bullet1::create(GameWorld* world, const Vec2& start_pos,
                            const Vec2& move_vec) {
    auto ob = world->newObject(layer_bullet, start_pos);
    ob->initWithSpriteFrameName("enemy_bullet_1.png");
    ob->setGameObjectType(object_type_bullet);

    auto ai = make_shared<Bullet1AI>(move_vec.x, move_vec.y);
    auto phy = make_shared<Bullet1Physics>(start_pos);

    ob->addGameComponent(ai);
    ob->addGameComponent(phy);

    WorldLight light(Color3B(194, 120, 194), 140, 1.0,
                     WorldLight::world_light_type2);

    ob->addWorldLight(light, "main_light");

    PhysicsShapeCache::getInstance()->setBodyOnSprite("enemy_bullet_1", ob);

    return ob;
}
