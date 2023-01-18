#ifndef __GAME_WORLD_H__
#define __GAME_WORLD_H__

#include <memory>
#include <set>
using namespace std;

#include "cocos2d.h"
using namespace cocos2d;

#include "game/game_frame/GameFrame.h"
#include "utility/QuadTree.h"
#include "utility/json/json.h"
#include "utility/math/iVec2.h"

class GameObject;
class GameMap;
class GameWorldRenderer;
class GameFrameManager;

class Connection;

enum ObjectLayer {
    layer_enemy = 1,
    layer_player,
    layer_bullet,
    layer_particle,
    layer_map_physics,
};

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

    void pushGameAct(const GameAct& act);

    virtual void cleanup() override;

    //////////////////////////////////////////////

    // 新建一个object
    GameObject* newObject(ObjectLayer layer, const Vec2& startPos);

    void removeObject(GameObject* ob);

    Quad<GameObject*>& get_objects() { return this->quad_tree; }

    void main_update_logic();

    void main_update_draw();

    Node* get_game_map_target() { return game_map_target; }

    Node* get_game_renderer_target() { return game_renderer_target; }

    Node* get_game_bk_target() { return game_bk_target; }

    // 摄像机跟随
    void camera_follow(GameObject* object) { camera_follow_object = object; }

    shared_ptr<Random> getGlobalRandom() { return this->_globalRandom; }

private:
    void processContact(PhysicsContact& conatct);
    void updateGameObjectPosition();

    void notice(const json& event);

private:
    float logic_update_dt;

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
    Node* game_bk_target;
    Quad<GameObject*> quad_tree;
    shared_ptr<GameMap> _gameMap;
    shared_ptr<GameWorldRenderer> _gameRenderer;

    queue<GameAct> _game_act_que;

    // 游戏内的任何粒子创建，随机等，都必须用此随机引擎，保证一致性
    shared_ptr<Random> _globalRandom;
};

class GameWorldRenderer {
public:
    virtual void init(Node* target) = 0;
    virtual void release() = 0;
    virtual void update(const Vec2& left_bottom, const Size& size,
                        GameWorld* gameworld) = 0;
    virtual Vec2 calcu_camera_speed(const Vec2& current_pos,
                                    const Vec2& target_pos) = 0;
};

#endif