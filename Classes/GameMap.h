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

#include "GameComponet.h"
#include "Random.h"
#include "iVec2.h"

class GameWorld;
class GameObject;

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

    shared_ptr<MapHelperComponent> _map_helper;
    shared_ptr<MapPhysicsComponent> _map_physics;

private:
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

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// 测试用

class MapGeneratorComponent1 : public MapGeneratorComponent {
public:
    void init(unsigned seed);

    virtual void generate(int w, int h, MapTile& map);

private:
    shared_ptr<Random> random;
};

class MapGeneratorComponent2 : public MapGeneratorComponent {
public:
    void init(unsigned seed);

    virtual void generate(int w, int h, MapTile& map);

private:
    shared_ptr<Random> random;
};

class MapPreRendererComponent1 : public MapPreRendererComponent {
private:
    struct MapArea {
        int x, y, w, h;
    };

public:
    virtual void init(MapTile* map) { this->init(map, 0); }

    void init(MapTile* map, unsigned seed);

    virtual bool isPreRenderFinish();

    virtual void preRender();

    // 返回整个地图的大小
    virtual Size afterPreRender(Node* target);

private:
    Texture2D* render(const MapArea& area);

    void createTile(MapTileType type, int bit_mask, const Vec2& pos,
                    vector<Node*>& dst);

    void createDecoration(int x, int y, MapTileType type, int bit_mask,
                          const Vec2& pos, const MapArea& area,
                          vector<Node*>& dst);

    void createTileMask(MapTileType type, int bit_mask, const Vec2& pos,
                        vector<Node*>& dst);

private:
    void createDec1(int x, int y, MapTileType type, int bit_mask,
                    const Vec2& pos, const MapArea& area, vector<Node*>& dst);
    void createDec2(int x, int y, MapTileType type, int bit_mask,
                    const Vec2& pos, const MapArea& area, vector<Node*>& dst);
    void createDec3(int x, int y, MapTileType type, int bit_mask,
                    const Vec2& pos, const MapArea& area, vector<Node*>& dst);
    void createDec4(int x, int y, MapTileType type, int bit_mask,
                    const Vec2& pos, const MapArea& area, vector<Node*>& dst);

private:
    shared_ptr<Random> _random;

    vector<bool> mark;

    MapTile* _map = nullptr;
    bool _isPreRenderFinish = false;

    int x_now, x_max, y_now, y_max;
    vector<pair<MapArea, Texture2D*>> textures;
};

class MapHelperComponent1 : public MapHelperComponent {
public:
    virtual iVec2 convert_in_map(const Vec2& pos) {
        const auto convert_in_map = [&](float num) {
            int k = ((int)num) / 64;
            if (k * 64 < num) {
                k += 1;
            }
            return k;
        };

        return iVec2(convert_in_map(pos.x), convert_in_map(pos.y));
    }
};

class MapPhysicsComponent1 : public MapPhysicsComponent {
public:
    virtual void updateLogic(GameWorld* game_world);

private:
    map<string, GameObject*> _dirty;
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

#endif