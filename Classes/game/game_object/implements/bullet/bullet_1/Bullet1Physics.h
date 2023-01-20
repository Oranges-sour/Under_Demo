#ifndef __BULLET_1_PHYSICS_H__
#define __BULLET_1_PHYSICS_H__

#include "game/game_object/GameObject.h"

class Bullet1Physics : public PhysicsComponent {
public:
    Bullet1Physics(const Vec2& pos) { this->posNow = pos; }

    virtual void updateLogicInScreenRect(GameObject* ob) override {}
    virtual void receiveGameAct(GameObject* ob, const GameAct& act) override {}
    virtual void receiveEvent(GameObject* ob, const json& event) override;

private:
    bool is_dead = false;
};

#endif