#include "Enemy1AI.h"

Enemy1AI::Enemy1AI(float detect_range, const string& bullet_json_key)
    : detect_range(detect_range), bullet_json_key(bullet_json_key) {}

void Enemy1AI::updateLogic(GameObject* ob) {
    auto world = ob->getGameWorld();
}
