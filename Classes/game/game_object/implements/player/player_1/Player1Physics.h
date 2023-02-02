#ifndef __PLAYER_1_PHYSICS_H__
#define __PLAYER_1_PHYSICS_H__

#include "game/game_object/GameObject.h"

class Player1Physics : public PhysicsComponent {
public:
    Player1Physics(const Vec2& start_pos, float move_speed, float jump_speed,
                   const vector<string>& frame_action_stay,
                   const vector<string>& frame_action_run,
                   const vector<string>& frame_action_jump,
                   const vector<string>& frame_action_jump_stay,
                   const vector<string>& frame_action_attack_near);

    virtual void receiveGameAct(GameObject* ob, const GameAct& act) override {}
    virtual void receiveEvent(GameObject* ob, const json& event) override;

private:
    void upd(GameObject* ob);

private:
    float move_speed;
    float jump_speed;

    float attack;
    bool on_attack;
    bool run;

    shared_ptr<GameObjectFrameAction> frame_stay;
    shared_ptr<GameObjectFrameAction> frame_run;
    shared_ptr<GameObjectFrameAction> frame_jump;
    shared_ptr<GameObjectFrameAction> frame_jump_stay;
    shared_ptr<GameObjectFrameAction> frame_attack_near;
};

class Player1Speed : public PhysicsComponent::SpeedComponent {
public:
    virtual void updateAfterEvent(GameObject* ob,
                                  PhysicsComponent* phy) override;

    void add_speed_rate(float rate, const string& key);

    void remove_speed_rate(const string& key);

private:
    map<string, float> speed_rate;
};

#endif