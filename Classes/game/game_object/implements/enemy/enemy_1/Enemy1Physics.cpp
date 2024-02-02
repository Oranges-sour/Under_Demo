#include "Enemy1Physics.h"

Enemy1Physics::Enemy1Physics(const Vec2& start_pos) {
    this->posNow = start_pos;

    this->schedule([&](GameObject* ob) { upd(ob); }, 0, "enemy_physics_upd");
}

void Enemy1Physics::receiveEvent(GameObject* ob, const GameEvent& event) {}

void Enemy1Physics::upd(GameObject* ob) {}
