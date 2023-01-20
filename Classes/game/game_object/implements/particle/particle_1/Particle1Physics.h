#ifndef __PARTICLE_1_PHYSICS_H__
#define __PARTICLE_1_PHYSICS_H__

#include "game/game_object/GameObject.h"

class Particle1Physics : public PhysicsComponent {
public:
    Particle1Physics(const Vec2& pos) { this->posNow = pos; }

    virtual void receiveGameAct(GameObject* ob, const GameAct& act) override {}
    virtual void receiveEvent(GameObject* ob, const json& event) override;
};

#endif