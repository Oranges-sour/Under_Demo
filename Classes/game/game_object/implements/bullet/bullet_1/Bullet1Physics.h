#ifndef __BULLET_1_PHYSICS_H__
#define __BULLET_1_PHYSICS_H__

#include "game/game_object/GameObject.h"

class Bullet1Physics : public PhysicsComponent {
public:
    Bullet1Physics(const Vec2& pos, const Vec2& direction, float move_speed,
                   float rotate_speed, int dead_particle_cnt,
                   const string& dead_particle_name);

    virtual void updateLogicInScreenRect(GameObject* ob) override {}
    virtual void receiveEvent(GameObject* ob, const json& event) override;

private:
    void upd(GameObject* ob);

private:
    int dead_particle_cnt;
    string dead_particle_name;
    Vec2 direction;
    float move_speed;
    float rotate_speed;

    bool is_dead;
};

#endif