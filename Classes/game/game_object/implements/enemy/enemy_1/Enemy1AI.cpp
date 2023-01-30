#include "Enemy1AI.h"

#include "game/game_map/GameMap.h"
#include "utility/math/MyMath.h"

Enemy1AI::Enemy1AI(float detect_range, const string& bullet_json_key,
                   int attack_speed)
    : detect_range(detect_range),
      bullet_json_key(bullet_json_key),
      attack_speed(attack_speed),
      cnt(0) {}

void Enemy1AI::updateLogic(GameObject* ob) {
    cnt += 1;

    auto world = ob->getGameWorld();
    auto& quad = world->getGameObjects();
    auto map = world->getGameMap();

    // ×óÏÂ
    auto p0 = map->getMapHelper()->convertInMap(
        ob->getPosition() - Vec2(detect_range, detect_range));
    // ÓÒÉÏ
    auto p1 = map->getMapHelper()->convertInMap(
        ob->getPosition() + Vec2(detect_range, detect_range));

    quad.visitInRect(
        {p0.x, p1.y}, {p1.x, p0.y}, [&](const iVec2&, GameObject* vis_ob) {
            if (vis_ob->getGameObjectType() != object_type_player) {
                return;
            }

            float dis =
                MyMath::distance(vis_ob->getPosition(), ob->getPosition());
        });
}
