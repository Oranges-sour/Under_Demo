#ifndef __BULLET_1_AI_H__
#define __BULLET_1_AI_H__

#include "game/game_object/GameObject.h"

class Bullet1AI : public GameComponent {
public:
    Bullet1AI(const Vec2& direction, float move_speed, float rotate_speed)
        : direction(direction),
          move_speed(move_speed),
          rotate_speed(rotate_speed) {}

    virtual void updateLogicInScreenRect(GameObject* ob) override {}
    virtual void updateLogic(GameObject* ob) override;
    virtual void updateDraw(GameObject* ob, float rate) override {}
    virtual void receiveGameAct(GameObject* ob, const GameAct& event) override {
    }
    virtual void receiveEvent(GameObject* ob, const json& event) override {}
    virtual void updateAfterEvent(GameObject* ob) override {}

private:
private:
    Vec2 direction;
    float move_speed;
    float rotate_speed;
};

#endif