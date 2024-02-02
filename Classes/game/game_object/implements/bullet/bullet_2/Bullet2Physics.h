#ifndef __BULLET_2_PHYSICS_H__
#define __BULLET_2_PHYSICS_H__

#include "game/game_object/GameObject.h"

class Bullet2Physics : public PhysicsComponent {
public:
public:
    Bullet2Physics(const Vec2& start_pos, const Vec2& end_pos,float move_speed,
                   int particle_explode_cnt,
                   const string& particle_explode_json_key);

    virtual void updateLogicInScreenRect(GameObject* ob) override {}
    virtual void receiveEvent(GameObject* ob, const json& event) override;

private:
    //void upd(GameObject* ob);

private:
    bool _is_dead;
    float _move_speed;
    string _particle_explode_json_key;
    int _particle_explode_cnt;
};

#endif
