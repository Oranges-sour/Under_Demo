#ifndef __PARTICLE_1_AI_H__
#define __PARTICLE_1_AI_H__

#include "game/game_object/GameObject.h"

class Particle1AI : public GameComponent {
public:
    Particle1AI(const Vec2& direction, int live_frame, float move_speed,
                float rotate_speed, float light_decrease_rate)
        : live_frame(live_frame),
          direction(direction),
          move_speed(move_speed),
          rotate_speed(move_speed),
          light_decrease_rate(light_decrease_rate) {}

    virtual void updateLogicInScreenRect(GameObject* ob) override {}
    virtual void updateLogic(GameObject* ob) override;
    virtual void updateDraw(GameObject* ob, float rate) override {}
    virtual void receiveGameAct(GameObject* ob, const GameAct& event){};
    virtual void receiveEvent(GameObject* ob, const json& event) override {}

private:
    Vec2 direction;

    int live_frame;
    float move_speed;
    float rotate_speed;
    float light_decrease_rate;

    int cnt = 0;
};

#endif