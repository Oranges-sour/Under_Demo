#pragma once
#ifndef __GAME_COMPONENT_H__
#define __GAME_COMPONENT_H__

#include "cocos2d.h"
using namespace cocos2d;

#include "json.h"

class GameAct;
class GameObject;

class GameComponent {
public:
    virtual void updateLogic(GameObject* ob) = 0;
    virtual void updateDraw(GameObject* ob, float rate) = 0;
    virtual void receiveGameAct(GameObject* ob, const GameAct& event) = 0;
    virtual void receiveEvent(GameObject* ob, const json& event) = 0;
};

class PhysicsComponent : public GameComponent {
public:
    PhysicsComponent() : scaleNow(Vec2(1.0, 1.0)), opacityNow(1.0) {}
    virtual void updateLogic(GameObject* ob) override;
    virtual void updateDraw(GameObject* ob, float rate) override;
    virtual void receiveGameAct(GameObject* ob, const GameAct& event) override {
    }
    virtual void receiveEvent(GameObject* ob, const json& event) {}

protected:
    ActionTween action_tween;
    EaseInOut action_ease;

    Vec2 scaleNow;
    Vec3 rotationNow;
    float opacityNow;
    Vec2 posNow;

    Vec2 scaleOld;
    Vec3 rotationOld;
    float opacityOld;
    Vec2 posOld;
};

#endif