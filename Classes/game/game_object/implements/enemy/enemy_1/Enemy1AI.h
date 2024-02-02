#ifndef __ENEMY_1_AI_H__
#define __ENEMY_1_AI_H__

#include "game/game_object/GameObject.h"

class Enemy1AI : public GameComponent {
public:
    Enemy1AI(float detect_range, const string& bullet_json_key,
             int attack_speed);

    virtual void updateLogicInScreenRect(GameObject* ob) override {}
    virtual void updateDraw(GameObject* ob, float rate) override {}
    virtual void receiveEvent(GameObject* ob, const json& event) override {}
    virtual void updateAfterEvent(GameObject* ob) override {}

private:
    void upd(GameObject* ob);

    void try_attack(GameObject* ob);

private:
    bool _can_attack;

    int _attack_speed;
    float _detect_range;
    string _bullet_json_key;
};

#endif