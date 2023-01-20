#include "Player1.h"

#include "game/game_object/GameObject.h"
#include "game/game_world/GameWorld.h"

#include "Player1AI.h"
#include "Player1Physics.h"
#include "utility/PhysicsShapeCache.h"

GameObject* Player1::create(GameWorld* world, const Vec2& start_pos,
                            const string& uid) {
    auto ob = world->newObject(layer_player, start_pos);

    ob->initWithSpriteFrameName("man_stay.png");
    ob->setGameObjectType(object_type_player);
    // Íæ¼ÒÓÐÌØÊâuid
    ob->setUID(uid);

    auto ai = make_shared<Player1AI>();
    auto phy = make_shared<Player1Physics>(start_pos);

    ob->addGameComponent(phy);
    ob->addGameComponent(ai);

    WorldLight light(Color3B(255, 255, 255), 600, 1.0,
                     WorldLight::world_light_type1);

    ob->addWorldLight(light, "main_light");

    PhysicsShapeCache::getInstance()->setBodyOnSprite("enemy_0", ob);

    return ob;
}
