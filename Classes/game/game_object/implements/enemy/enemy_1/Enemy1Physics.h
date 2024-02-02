#ifndef __ENEMY_1_PHYSICS_H__
#define __ENEMY_1_PHYSICS_H__

#include "game/game_object/GameObject.h"

class Enemy1Physics : public PhysicsComponent {
public:
    Enemy1Physics(const Vec2& start_pos);

    virtual void receiveEvent(GameObject* ob, const GameEvent& event) override;

private:
    void upd(GameObject* ob);
};

#endif