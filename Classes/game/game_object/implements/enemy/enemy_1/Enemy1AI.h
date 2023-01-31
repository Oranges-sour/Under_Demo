#ifndef __ENEMY_1_AI_H__
#define __ENEMY_1_AI_H__

#include "game/game_object/GameObject.h"

class Enemy1AI : public GameComponent {
public:
    Enemy1AI(float detect_range, const string& bullet_json_key,
             int attack_speed);

    virtual void updateLogicInScreenRect(GameObject* ob) override {}
    virtual void updateDraw(GameObject* ob, float rate) override {}
    virtual void receiveGameAct(GameObject* ob, const GameAct& event) override {
    }
    virtual void receiveEvent(GameObject* ob, const json& event) override {}
    virtual void updateAfterEvent(GameObject* ob) override {}

private:
    void upd(GameObject* ob);

private:
    int attack_speed;
    float detect_range;
    string bullet_json_key;
};

#endif