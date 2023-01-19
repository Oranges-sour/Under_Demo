#pragma once
#ifndef __GAME_MAP_H__
#define __GAME_MAP_H__

#include "cocos2d.h"
using namespace cocos2d;

#include <map>
#include <memory>
#include <string>
#include <vector>
using namespace std;

#include "game/game_object/GameObject.h"
#include "utility/math/Random.h"
#include "utility/math/iVec2.h"

class GameWorld;

class MapGeneratorComponent;
class MapHelperComponent;
class MapPreRendererComponent;
class MapPhysicsComponent;

enum MapTileType { air = 0, rock, grass, dirt };

class MapTileHelper final {
public:
    MapTileType& operator[](int y) { return map[y * w + x]; }

private:
    friend class MapTile;
    MapTileHelper(vector<MapTileType>& map, int x, int w)
        : map(map), x(x), w(w) {}
    int x, w;
    vector<MapTileType>& map;
};

class MapTile final {
public:
    MapTile(int w, int h) : w(w), h(h) { map.resize((w + 1) * (h + 1)); }

    MapTileHelper operator[](int x) { return MapTileHelper(map, x, w); }

    int w, h;
    vector<MapTileType> map;
};

// 地图
class GameMap final {
public:
    GameMap(int w, int h) : map(w, h), w(w), h(h) {
        assert(w % 16 == 0);
        assert(h % 16 == 0);
    }
    void init(shared_ptr<MapGeneratorComponent> mapGenerator,
              shared_ptr<MapHelperComponent> mapHelper,
              shared_ptr<MapPhysicsComponent> mapPhysics);

    MapTile& get() { return map; }

    void updateLogic(GameWorld* game_world);

    shared_ptr<MapHelperComponent> getMapHelper() { return this->_map_helper; }

    shared_ptr<MapPhysicsComponent> getMapPhysics() {
        return this->_map_physics;
    }

private:
    shared_ptr<MapHelperComponent> _map_helper;
    shared_ptr<MapPhysicsComponent> _map_physics;
    int w, h;
    // 方便对应，不使用下标0
    MapTile map;
};

// 地图生成器
class MapGeneratorComponent {
public:
    virtual void generate(int w, int h, MapTile& map) = 0;
};

// 地图预渲染
class MapPreRendererComponent {
public:
    virtual void init(MapTile* map) = 0;

    virtual bool isPreRenderFinish() = 0;

    virtual void preRender() = 0;

    // 返回整个地图的大小
    virtual Size afterPreRender(Node* target) = 0;
};

class MapDecorationCreatorComponent {
public:
    virtual void generate(GameWorld* world, MapTile* map) = 0;
};

class MapGameObjectTileComponent : public GameComponent {
public:
    void init(map<string, GameObject*>* _dirty, const string& uid) {
        this->_dirty = _dirty;
        this->uid = uid;
    }

    virtual void updateLogic(GameObject* ob);
    virtual void updateDraw(GameObject* ob, float rate) {}
    virtual void receiveEvent(GameObject* ob, const json& event);
    virtual void receiveGameAct(GameObject* ob, const GameAct& act) {}

private:
    string uid;
    map<string, GameObject*>* _dirty;
    int cnt = 10;
};

// 地图附件
class MapHelperComponent {
public:
    virtual iVec2 convert_in_map(const Vec2& pos) = 0;
};

// 地图物理组件
class MapPhysicsComponent {
public:
    virtual void updateLogic(GameWorld* game_world) = 0;
};

#endif