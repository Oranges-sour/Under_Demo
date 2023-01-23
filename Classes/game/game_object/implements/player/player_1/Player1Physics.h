#ifndef __PLAYER_1_PHYSICS_H__
#define __PLAYER_1_PHYSICS_H__

#include "game/game_object/GameObject.h"

class Player1Physics : public PhysicsComponent {
public:
    Player1Physics(const Vec2& start_pos, float move_speed, float jump_speed,
                   const vector<string>& frame_action_stay,
                   const vector<string>& frame_action_run,
                   const vector<string>& frame_action_jump,
                   const vector<string>& frame_action_jump_stay);

    virtual void receiveGameAct(GameObject* ob, const GameAct& act) override {}
    virtual void receiveEvent(GameObject* ob, const json& event) override;
    virtual void updateLogic(GameObject* ob) override;

private:
    float move_speed;
    float jump_speed;
    bool run;

    shared_ptr<GameObjectFrameAction> frame_stay;
    shared_ptr<GameObjectFrameAction> frame_run;
    shared_ptr<GameObjectFrameAction> frame_jump;
    shared_ptr<GameObjectFrameAction> frame_jump_stay;
};

#endif