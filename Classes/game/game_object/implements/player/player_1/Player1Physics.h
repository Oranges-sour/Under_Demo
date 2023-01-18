#ifndef __PLAYER_1_PHYSICS_H__
#define __PLAYER_1_PHYSICS_H__

#include "game/game_object/GameObject.h"

class Player1Physics : public PhysicsComponent {
public:
    Player1Physics(const Vec2& pos) { this->posNow = pos; }

    virtual void receiveGameAct(GameObject* ob, const GameAct& act) override {}
    virtual void receiveEvent(GameObject* ob, const json& event) override;
    virtual void updateLogic(GameObject* ob) override;

private:
    float fall_speed_y = 0;

    void wall_contact_check(GameObject* ob);
};

#endif