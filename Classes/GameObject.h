#pragma once
#ifndef __GAME_OBKJECT_H__
#define __GAME_OBKJECT_H__

#include "cocos2d.h"
using namespace cocos2d;

#include <memory>
#include <queue>
using namespace std;

#include "GameWorld.h"
#include "json.h"

#include "QuadTree.h"

class PhysicsComponent;
class GameComponent;

enum GameObjectType {
    player,
    bullet,
    particle,
    enemy,
    equipment,
    decorate,
    unknow,
};

class GameObject : public Sprite, public ActionTweenDelegate {
public:
    // 由GameWorld调用
    bool init(GameWorld* game_world);

    virtual void removeFromParent() override;

    GameObject* getNext() const;
    void setNext(GameObject* game_object);

    bool isInUse();
    void setInUse(bool state);

    void addGameComponent(shared_ptr<GameComponent> componet);

    void pushEvent(const json& event);

    virtual void updateTweenAction(float rate, const std::string& key) override;

    //灯光
    void setLightColor(const Color3B& color) { this->light_color = color; }

    const Color3B& getLightColor() { return this->light_color; }

    void setLightRadius(float r) { this->light_radius = r; }

    float getLightRadius() { return this->light_radius; }
    //////////////////////////////////

    void main_update();

    void setGameObjectType(GameObjectType new_type) {
        _game_object_type = new_type;
    }

    GameObjectType getGameObjectType() { return this->_game_object_type; }

    GameWorld* get_game_world() { return this->game_world; }

    Quad_node<GameObject*> quad_node;

private:
    GameWorld* game_world;

    GameObjectType _game_object_type = unknow;

    Color3B light_color;
    float light_radius;

    bool _inUse;
    GameObject* _next;

    vector<shared_ptr<GameComponent>> _componets;

    queue<json> _componetEventQueue;

    ActionTween _actionTween;
    EaseInOut _actionEase;
};

#endif