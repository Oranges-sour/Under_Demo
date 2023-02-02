#pragma once
#ifndef __GAME_OBKJECT_H__
#define __GAME_OBKJECT_H__

#include "cocos2d.h"
using namespace cocos2d;

#include <climits>
#include <functional>
#include <map>
#include <memory>
#include <queue>
using namespace std;

#include "game/game_frame/GameFrame.h"
#include "game/game_world/GameWorld.h"
#include "utility/QuadTree.h"
#include "utility/json/json.h"

class PhysicsComponent;
class GameComponent;
class GameObjectFrameAction;

enum GameObjectType {
    object_type_player,
    object_type_bullet,
    object_type_particle,
    object_type_enemy,
    object_type_equipment,
    object_type_decoration,
    object_type_wall,  // 地图的块
    object_type_unknow,
};

struct WorldLight {
    static void setWorldLight(const vector<json>& lights, GameObject* ob);

    enum WorldLightType {
        world_light_type1 = 1,
        world_light_type2,
        world_light_type3
    };

    WorldLight()
        : lightColor(Color3B(255, 255, 255)),
          radius(100.0f),
          opacity(1.0f),
          type(world_light_type1),
          offset(0, 0) {}
    WorldLight(const Color3B& color, float radius, float opacity,
               WorldLightType type, const Vec2& offset = Vec2::ZERO)
        : lightColor(color),
          radius(radius),
          opacity(opacity),
          type(type),
          offset(offset) {}

    Color3B lightColor;
    float radius;
    float opacity;
    Vec2 offset;
    WorldLightType type;
};

class GameObject : public Sprite, public ActionTweenDelegate {
public:
    static GameObject* create();

    // 由GameWorld调用
    bool init(GameWorld* game_world);

    void setUID(const string& new_uid) { this->_uid = new_uid; }

    const string& getUID() { return this->_uid; }

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

    map<string, WorldLight>& getAllWorldLight() { return _world_light; }
    //////////////////////////////////

    void mainUpdate();

    // 在屏幕显示区域内会调用
    void mainUpdateInScreenRect();

    void setGameObjectType(GameObjectType new_type) {
        _game_object_type = new_type;
    }

    GameObjectType getGameObjectType() { return this->_game_object_type; }

    GameWorld* getGameWorld() { return this->_game_world; }

    QuadNode<GameObject*> _quad_node;

    // 帧动画
    void switchFrameActionStatue(shared_ptr<GameObjectFrameAction> new_action);

private:
    shared_ptr<GameObjectFrameAction> _frame_action;

    string _uid;

    GameWorld* _game_world;

    GameObjectType _game_object_type = object_type_unknow;

    map<string, WorldLight> _world_light;

    vector<shared_ptr<GameComponent>> _componets;

    queue<GameAct> _componet_game_act_queue;
    queue<json> _componet_event_queue;

    ActionTween _actionTween;
    EaseInOut _actionEase;
};

class GameComponent {
public:
    void schedule(const function<void(GameObject* ob)>& call_back, int interval,
                  const string& key, int repeat_time = INT_MAX,
                  int first_run_delay = 0, int order = 0);
    void scheduleOnce(const function<void(GameObject* ob)>& call_back,
                      int delay, const string& key);
    void unschedule(const string& key);

    void updateLogic(GameObject* ob);

    virtual void updateLogicInScreenRect(GameObject* ob) = 0;
    virtual void updateDraw(GameObject* ob, float rate) = 0;
    virtual void receiveGameAct(GameObject* ob, const GameAct& event) = 0;
    virtual void receiveEvent(GameObject* ob, const json& event) = 0;
    virtual void updateAfterEvent(GameObject* ob) = 0;

private:
    struct ScheduleBag {
        ScheduleBag(const function<void(GameObject* ob)>& call_back,
                    int interval, int repeat_time, int first_run_delay,
                    int order, const string& key)
            : _call_back(call_back),
              _interval(interval),
              _repeat_time(repeat_time),
              _first_run_delay(first_run_delay),
              _order(order),
              _key(key),
              _cnt(interval) {}
        function<void(GameObject* ob)> _call_back;
        int _interval;
        int _repeat_time;
        int _first_run_delay;
        int _order;
        string _key;

        int _cnt;
    };

    vector<ScheduleBag> _schedule_need_to_add;
    vector<string> _schedule_need_to_erase;
    map<int, map<string, ScheduleBag>> _schedule_bag;
};

class GameObjectFrameAction {
public:
    GameObjectFrameAction(
        const vector<string>& sprite_frames,
        const function<void(GameObject*, int)>& round_call_back);

    void reset();

    void play(GameObject* ob);

private:
    int round_cnt;
    int cnt;
    function<void(GameObject*, int)> round_call_back;
    vector<string> sprite_frames;
};

#include "game/game_object/implements/physics_component/PhysicsComponent.h"

#endif