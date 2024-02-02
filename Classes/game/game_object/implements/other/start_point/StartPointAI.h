#ifndef __START_POINT_AI_H__
#define __START_POINT_AI_H__

#include "game/game_object/GameObject.h"

class StartPointAI : public GameComponent {
public:
    StartPointAI(float particle_cnt_per_frame, const string& particle_name,
                 const Vec2& offset_1, const Vec2& offset_2);

    virtual void updateLogicInScreenRect(GameObject* ob) override;
    virtual void updateDraw(GameObject* ob, float rate) override {}
    virtual void receiveEvent(GameObject* ob, const GameEvent& event) override {}
    virtual void updateAfterEvent(GameObject* ob) override {}

private:
    float sum;

private:
    Vec2 offset_1;
    Vec2 offset_2;
    float particle_cnt_per_frame;
    string particle_name;
};

#endif