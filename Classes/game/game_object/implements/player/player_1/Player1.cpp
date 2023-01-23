#include "Player1.h"

#include "Player1AI.h"
#include "Player1Physics.h"
#include "game/game_object/GameObject.h"
#include "game/game_world/GameWorld.h"
#include "utility/PhysicsShapeCache.h"
#include "utility/GameObjectInfo.h"

GameObject* Player1::create(GameWorld* world, const json& json_key,
                            const Vec2& start_pos, const string& uid) {
    auto& json = GameObjectInfo::instance()->get(json_key);


    auto ob = world->newObject(layer_player, start_pos);
    ob->initWithSpriteFrameName("man_stay.png");
    ob->setGameObjectType(object_type_player);
    // Íæ¼ÒÓÐÌØÊâuid
    ob->setUID(uid);

    auto ai = make_shared<Player1AI>();
    auto phy = make_shared<Player1Physics>(start_pos);

    auto speed_comp = make_shared<PhysicsComponent::SpeedComponent>();
    auto gravity_comp =
        make_shared<PhysicsComponent::GravityComponent>(6, speed_comp);
    auto wall_contact_comp =
        make_shared<PhysicsComponent::WallContactComponent>(
            Vec2(-30, -135), Vec2(30, -15), speed_comp);

    phy->setGravityComponent(gravity_comp);
    phy->setSpeedComponent(speed_comp);
    phy->setWallContactComponent(wall_contact_comp);

    ob->addGameComponent(phy);
    ob->addGameComponent(ai);

    WorldLight light(Color3B(255, 255, 255), 600, 1.0,
                     WorldLight::world_light_type1);

    ob->addWorldLight(light, "main_light");

    PhysicsShapeCache::getInstance()->setBodyOnSprite("enemy_0", ob);

    return ob;
}
