#ifndef __BULLET_2_AI_H__
#define __BULLET_2_AI_H__

#include "game/game_object/GameObject.h"

class Bullet2AI : public GameComponent {
public:
    Bullet2AI(const Vec2& start_pos, const Vec2& end_pos,
              const string& particle_move_json_key,
              float particle_move_cnt_per_frame);

    virtual void updateLogicInScreenRect(GameObject* ob) override {}
    virtual void updateDraw(GameObject* ob, float rate) override {}
    virtual void receiveEvent(GameObject* ob, const json& event) override {}
    virtual void updateAfterEvent(GameObject* ob) override {}

private:
    void upd(GameObject* ob);

    void create_particle(GameObject* ob);

private:
    float _particle_move_cnt;

    Vec2 _direction;
    float _particle_move_cnt_per_frame;
    string _particle_move_json_key;
};

#endif
