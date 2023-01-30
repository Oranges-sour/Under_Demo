#include "Enemy1Physics.h"

Enemy1Physics::Enemy1Physics() {
    this->schedule([&](GameObject* ob) { upd(ob); }, 0, "enemy_physics_upd");
}

void Enemy1Physics::receiveEvent(GameObject* ob, const json& event) {}

void Enemy1Physics::upd(GameObject* ob) {}
