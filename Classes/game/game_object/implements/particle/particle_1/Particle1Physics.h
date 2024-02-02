#ifndef __PARTICLE_1_PHYSICS_H__
#define __PARTICLE_1_PHYSICS_H__

#include "game/game_object/GameObject.h"

class Particle1Physics : public PhysicsComponent {
public:
    Particle1Physics(const Vec2& pos, float scale_decrease_rate,
                     float opacity_decrease_rate);

    virtual void receiveEvent(GameObject* ob, const GameEvent& event) override;

private:
    void upd(GameObject* ob);

private:
    float scale_decrease_rate;
    float opacity_decrease_rate;
};

#endif