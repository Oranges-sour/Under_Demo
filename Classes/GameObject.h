#pragma once
#ifndef __GAME_OBKJECT_H__
#define __GAME_OBKJECT_H__

#include "cocos2d.h"
using namespace cocos2d;

#include <memory>
#include <queue>
using namespace std;

#include "GameWorld.h"
#include "QuadTree.h"
#include "json.h"
#include "GameFrame.h"

class PhysicsComponent;
class GameComponent;

enum GameObjectType {
    object_type_player,
    object_type_bullet,
    object_type_particle,
    object_type_enemy,
    object_type_equipment,
    object_type_decorate,
    object_type_unknow,
};

class GameObject : public Sprite, public ActionTweenDelegate {
public:
    // 由GameWorld调用
    bool init(GameWorld* game_world);

    void setUID(const string& new_uid) { this->uid = new_uid; }

    const string& getUID() { return this->uid; }

    virtual void removeFromParent() override;

    GameObject* getNext() const;
    void setNext(GameObject* game_object);

    bool isInUse();
    void setInUse(bool state);

    void addGameComponent(shared_ptr<GameComponent> componet);

    void pushEvent(const json& event);
    void pushGameAct(const GameAct& act);

    virtual void updateTweenAction(float rate, const std::string& key) override;

    // 灯光
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
    string uid;

    GameWorld* game_world;

    GameObjectType _game_object_type = object_type_unknow;

    Color3B light_color;
    float light_radius;

    bool _inUse;
    GameObject* _next;

    vector<shared_ptr<GameComponent>> _componets;

    queue<GameAct> _componetGameActQueue;
    queue<json> _componetEventQueue;

    ActionTween _actionTween;
    EaseInOut _actionEase;
};

#endif