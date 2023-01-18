#ifndef	__BULLET_1_AI_H__
#define __BULLET_1_AI_H__

#include "game/game_object/GameObject.h"

class Bullet1AI : public GameComponent {
public:
    Bullet1AI(float x, float y) : xx(x), yy(y) {}

    virtual void updateLogic(GameObject* ob) override;
    virtual void updateDraw(GameObject* ob, float rate) override {}
    virtual void receiveGameAct(GameObject* ob, const GameAct& event){};
    virtual void receiveEvent(GameObject* ob, const json& event) override {}

private:
    float xx, yy;
};

#endif