#ifndef __BULLET_1_PHYSICS_H__
#define __BULLET_1_PHYSICS_H__

#include "game/game_object/GameObject.h"

class Bullet1Physics : public PhysicsComponent {
public:
    Bullet1Physics(const Vec2& pos, int dead_particle_cnt,
                   const string& dead_particle_name)
        : is_dead(false),
          dead_particle_cnt(dead_particle_cnt),
          dead_particle_name(dead_particle_name) {
        this->posNow = pos;
    }

    virtual void updateLogicInScreenRect(GameObject* ob) override {}
    virtual void receiveGameAct(GameObject* ob, const GameAct& act) override {}
    virtual void receiveEvent(GameObject* ob, const json& event) override;

private:
    int dead_particle_cnt;
    string dead_particle_name;

    bool is_dead;
};

#endif