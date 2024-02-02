#ifndef __PLAYER_1_AI_H__
#define __PLAYER_1_AI_H__

#include "game/game_object/GameObject.h"

class Player1AI : public GameComponent {
public:
    Player1AI();

    virtual void updateLogicInScreenRect(GameObject* ob) override {}
    virtual void updateDraw(GameObject* ob, float rate) override {}
    virtual void receiveEvent(GameObject* ob, const GameEvent& event) override;
    virtual void updateAfterEvent(GameObject* ob) override {}
};

#endif