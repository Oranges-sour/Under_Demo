#pragma once
#ifndef __GAME_OBKJECT_H__
#define __GAME_OBKJECT_H__

#include "cocos2d.h"
using namespace cocos2d;

#include <memory>
#include <queue>
using namespace std;

#include "game/game_frame/GameFrame.h"
#include "game/game_world/GameWorld.h"
#include "utility/QuadTree.h"
#include "utility/json/json.h"

class PhysicsComponent;
class GameComponent;

enum GameObjectType {
    object_type_player,
    object_type_bullet,
    object_type_particle,
    object_type_enemy,
    object_type_equipment,
    object_type_decorate,
    object_type_wall,  // 地图的块
    object_type_unknow,
};

struct WorldLight {
    enum WorldLightType {
        world_light_type1,
        world_light_type2,
        world_light_type3
    };

    WorldLight()
        : lightColor(Color3B(255, 255, 255)),
          radius(100.0f),
          opacity(1.0f),
          type(world_light_type1) {}
    WorldLight(const Color3B& color, float radius, float opacity,
               WorldLightType type)
        : lightColor(color), radius(radius), opacity(opacity), type(type) {}

    Color3B lightColor;
    float radius;
    float opacity;
    WorldLightType type;
};

class GameObject : public Sprite, public ActionTweenDelegate {
public:
    static GameObject* create();

    // 由GameWorld调用
    bool init(GameWorld* game_world);

    void setUID(const string& new_uid) { this->uid = new_uid; }

    const string& getUID() { return this->uid; }

    virtual void removeFromParent() override;

    void addGameComponent(shared_ptr<GameComponent> componet);

    void pushEvent(const json& event);
    void pushGameAct(const GameAct& act);

    virtual void updateTweenAction(float rate, const std::string& key) override;

    // 灯光
    void addWorldLight(const WorldLight& light, const string& key) {
        _world_light.insert({key, light});
    }

    void removeWorldLight(const string& key) { _world_light.erase(key); }

    WorldLight* getWorldLight(const string& key) {
        auto iter = _world_light.find(key);
        if (iter == _world_light.end()) {
            return nullptr;
        }
        return &(iter->second);
    }

    const map<string, WorldLight>& getAllWorldLight() { return _world_light; }
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

    map<string, WorldLight> _world_light;

    vector<shared_ptr<GameComponent>> _componets;

    queue<GameAct> _componetGameActQueue;
    queue<json> _componetEventQueue;

    ActionTween _actionTween;
    EaseInOut _actionEase;
};

class GameComponent {
public:
    virtual void updateLogic(GameObject* ob) = 0;
    virtual void updateDraw(GameObject* ob, float rate) = 0;
    virtual void receiveGameAct(GameObject* ob, const GameAct& event) = 0;
    virtual void receiveEvent(GameObject* ob, const json& event) = 0;
};

class PhysicsComponent : public GameComponent {
public:
    PhysicsComponent()
        : scaleNow(Vec2(1.0, 1.0)), opacityNow(1.0), rotationNow(0.0) {}
    virtual void updateLogic(GameObject* ob) override;
    virtual void updateDraw(GameObject* ob, float rate) override;
    virtual void receiveGameAct(GameObject* ob, const GameAct& event) override {
    }
    virtual void receiveEvent(GameObject* ob, const json& event) {}

protected:
    ActionTween action_tween;
    EaseInOut action_ease;

    Vec2 scaleNow;
    float rotationNow;
    float opacityNow;
    Vec2 posNow;

    Vec2 scaleOld;
    float rotationOld;
    float opacityOld;
    Vec2 posOld;
};

#endif