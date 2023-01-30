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
    MapTileType& operator[](int y) { return _map[y * _w + _x]; }

private:
    friend class MapTile;
    MapTileHelper(vector<MapTileType>& map, int x, int w)
        : _map(map), _x(x), _w(w) {}
    int _x, _w;
    vector<MapTileType>& _map;
};

class MapTile final {
public:
    MapTile(int w, int h) : _w(w), _h(h) { _map.resize((w + 1) * (h + 1)); }

    MapTileHelper operator[](int x) { return MapTileHelper(_map, x, _w); }

    int _w, _h;

private:
    vector<MapTileType> _map;
};

// 地图
class GameMap final {
public:
    GameMap(int w, int h) : _map(w, h), _w(w), _h(h) {
        assert(w % 16 == 0);
        assert(h % 16 == 0);
    }
    void init(shared_ptr<MapGeneratorComponent> mapGenerator,
              shared_ptr<MapHelperComponent> mapHelper,
              shared_ptr<MapPhysicsComponent> mapPhysics);

    MapTile& get() { return _map; }

    void updateLogic(GameWorld* game_world);

    shared_ptr<MapHelperComponent> getMapHelper() { return this->_map_helper; }

    shared_ptr<MapPhysicsComponent> getMapPhysics() {
        return this->_map_physics;
    }

private:
    shared_ptr<MapHelperComponent> _map_helper;
    shared_ptr<MapPhysicsComponent> _map_physics;
    int _w, _h;
    // 方便对应，不使用下标0
    MapTile _map;
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
        this->_uid = uid;
        _cnt = 10;
    }

    virtual void updateLogicInScreenRect(GameObject* ob) override {}
    virtual void updateLogic(GameObject* ob);
    virtual void updateDraw(GameObject* ob, float rate) {}
    virtual void receiveEvent(GameObject* ob, const json& event);
    virtual void receiveGameAct(GameObject* ob, const GameAct& act) {}
    virtual void updateAfterEvent(GameObject* ob) override {}

private:
    string _uid;
    map<string, GameObject*>* _dirty;
    int _cnt;
};

// 地图附件
class MapHelperComponent {
public:
    virtual iVec2 convertInMap(const Vec2& pos) = 0;
};

// 地图物理组件
class MapPhysicsComponent {
public:
    virtual void updateLogic(GameWorld* game_world) = 0;
};

#endif