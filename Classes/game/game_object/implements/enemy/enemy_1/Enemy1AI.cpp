#include "Enemy1AI.h"

#include "game/game_map/GameMap.h"
#include "game/game_object/implements/bullet/bullet_2/Bullet2.h"
#include "utility/math/MyMath.h"

Enemy1AI::Enemy1AI(float detect_range, const string& bullet_json_key,
                   int attack_speed)
    : _detect_range(detect_range),
      _bullet_json_key(bullet_json_key),
      _attack_speed(attack_speed),
      _can_attack(true) {
    this->schedule([&](GameObject* ob) { upd(ob); }, 0, "enemy_ai");
}

void Enemy1AI::upd(GameObject* ob) { try_attack(ob); }

void Enemy1AI::try_attack(GameObject* ob) {
    if (!_can_attack) {
        return;
    }

    auto world = ob->getGameWorld();
    auto& quad = world->getGameObjects();
    auto map = world->getGameMap();

    // ×óÏÂ
    auto p0 = map->getMapHelper()->convertInMap(
        ob->getPosition() - Vec2(_detect_range, _detect_range));
    // ÓÒÉÏ
    auto p1 = map->getMapHelper()->convertInMap(
        ob->getPosition() + Vec2(_detect_range, _detect_range));

    bool is_attack = false;

    quad.visitInRect(
        {p0.x, p1.y}, {p1.x, p0.y}, [&](const iVec2&, GameObject* vis_ob) {
            if (vis_ob->getGameObjectType() != object_type_player) {
                return;
            }

            float dis =
                MyMath::distance(vis_ob->getPosition(), ob->getPosition());

            if (dis < _detect_range) {
                is_attack = true;

                Bullet2::create(world, _bullet_json_key, ob->getPosition(),
                                vis_ob->getPosition() - Vec2(0, 40));
            }
        });

    if (is_attack) {
        _can_attack = false;
        this->scheduleOnce([&](GameObject* ob) { _can_attack = true; },
                           _attack_speed, "reset_attack");
    }
}
