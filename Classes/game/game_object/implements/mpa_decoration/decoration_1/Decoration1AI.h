#ifndef __MAP_DECORATION_1_AI_H__
#define __MAP_DECORATION_1_AI_H__

#include "game/game_object/GameObject.h"

class MapDecoration1AI : public GameComponent {
public:
    virtual void updateLogicInScreenRect(GameObject* ob) override {}
    virtual void updateDraw(GameObject* ob, float rate) override {}
    virtual void receiveGameAct(GameObject* ob, const GameAct& event){};
    virtual void receiveEvent(GameObject* ob, const json& event) override {}
    virtual void updateAfterEvent(GameObject* ob) override {}
};

#endif