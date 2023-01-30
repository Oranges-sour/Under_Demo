#ifndef __PARTICLE_1_AI_H__
#define __PARTICLE_1_AI_H__

#include "game/game_object/GameObject.h"

class Particle1AI : public GameComponent {
public:
    Particle1AI(const Vec2& direction, int live_frame, float move_speed,
                float rotate_speed, float light_decrease_rate);

    virtual void updateLogicInScreenRect(GameObject* ob) override {}
    virtual void updateDraw(GameObject* ob, float rate) override {}
    virtual void receiveGameAct(GameObject* ob, const GameAct& event){};
    virtual void receiveEvent(GameObject* ob, const json& event) override {}
    virtual void updateAfterEvent(GameObject* ob) override {}

private:
    void upd(GameObject* ob);

private:
    Vec2 direction;

    int live_frame;
    float move_speed;
    float rotate_speed;
    float light_decrease_rate;

    int cnt = 0;
};

#endif