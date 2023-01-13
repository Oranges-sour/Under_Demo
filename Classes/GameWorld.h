#ifndef __GAME_WORLD_H__
#define __GAME_WORLD_H__

#include <memory>
#include <set>
using namespace std;

#include "cocos2d.h"
using namespace cocos2d;

#include "QuadTree.h"
#include "iVec2.h"

#include "json.h"

class GameObject;
class GameMap;
class GameWorldRenderer;
class GameFrameManager;

class Connection;

class GameWorld : public Node {
public:
    GameWorld() : quad_tree({1, 256}, {256, 1}) {}

    static GameWorld* create();

    virtual bool init() override;

    ///////////////////////////////////////////
    void setGameMap(shared_ptr<GameMap> gameMap) { this->_gameMap = gameMap; }

    shared_ptr<GameMap> getGameMap() { return this->_gameMap; }

    void setGameRenderer(shared_ptr<GameWorldRenderer> gameRenderer) {
        this->_gameRenderer = gameRenderer;
    }

    shared_ptr<GameWorldRenderer> getGameRenderer() {
        return this->_gameRenderer;
    }

    void setGameFrameManager(shared_ptr<GameFrameManager> frameManager) {
        this->_frameManager = frameManager;
    }

    shared_ptr<GameFrameManager> getGameFrameManager() {
        return this->_frameManager;
    }

    virtual void cleanup() override;

    //////////////////////////////////////////////

    // 新建一个object
    GameObject* newObject(int layer, const Vec2& startPos);

    void removeObject(GameObject* ob);

    Quad<GameObject*>& get_objects() { return this->quad_tree; }

    void main_update_logic();

    void main_update_draw();

    Node* get_game_map_target() { return game_map_target; }

    Node* get_game_renderer_atrget() { return game_renderer_target; }

    // 摄像机跟随
    void camera_follow(GameObject* object) { camera_follow_object = object; }

    shared_ptr<Random> getGlobalRandom() { return this->_globalRandom; }

private:
    void processContact(PhysicsContact& conatct);
    void updateGameObjectPosition();

    void notice(const json& event);

private:
    set<GameObject*> needToRemove;
    map<GameObject*, iVec2> needToAdd;

    map<string, GameObject*> _game_objects;

    // 摄像机的中心点，坐标为地图中坐标
    Vec2 camera_pos;
    // 摄像机的目标坐标，实现缓动效果
    Vec2 camera_pos_target;

    GameObject* camera_follow_object = nullptr;

    Node* game_map_target;
    Node* game_renderer_target;
    Node* _game_node;
    Quad<GameObject*> quad_tree;
    shared_ptr<GameMap> _gameMap;
    shared_ptr<GameWorldRenderer> _gameRenderer;
    shared_ptr<GameFrameManager> _frameManager;

    // 游戏内的任何粒子创建，随机等，都必须用此随机引擎，保证一致性
    shared_ptr<Random> _globalRandom;
};

class GameWorldRenderer {
public:
    virtual void init(Node* target) = 0;
    virtual void update(const Vec2& left_bottom, const Size& size,
                        GameWorld* gameworld) = 0;
    virtual Vec2 calcu_camera_speed(const Vec2& current_pos,
                                    const Vec2& target_pos) = 0;
};

class GameWorldRenderer1 : public GameWorldRenderer {
public:
    ~GameWorldRenderer1();

    virtual void init(Node* target);
    virtual void update(const Vec2& left_bottom, const Size& size,
                        GameWorld* gameworld);
    virtual Vec2 calcu_camera_speed(const Vec2& current_pos,
                                    const Vec2& target_pos);

private:
    Sprite* light = nullptr;
    RenderTexture* render = nullptr;
    vector<Sprite*> lights;
};

#endif