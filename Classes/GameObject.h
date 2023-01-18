#pragma once
#ifndef __GAME_OBKJECT_H__
#define __GAME_OBKJECT_H__

#include "cocos2d.h"
using namespace cocos2d;

#include <memory>
#include <queue>
using namespace std;

#include "GameFrame.h"
#include "GameWorld.h"
#include "QuadTree.h"
#include "json.h"

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

#endif