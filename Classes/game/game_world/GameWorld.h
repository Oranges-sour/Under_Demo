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
    layer_map_decoration = 1,
    layer_enemy,
    layer_player,
    layer_bullet,
    layer_particle,
    layer_map_physics,
};

class GameWorld : public Node {
public:
    GameWorld() : _quad_tree({1, 256}, {256, 1}) {}

    static GameWorld* create();

    virtual bool init() override;

    ///////////////////////////////////////////
    void setGameMap(shared_ptr<GameMap> gameMap) { this->_game_map = gameMap; }

    shared_ptr<GameMap> getGameMap() { return this->_game_map; }

    void setGameRenderer(shared_ptr<GameWorldRenderer> gameRenderer) {
        this->_game_renderer = gameRenderer;
    }

    shared_ptr<GameWorldRenderer> getGameRenderer() {
        return this->_game_renderer;
    }

    void pushGameAct(const GameAct& act);

    virtual void cleanup() override;

    //////////////////////////////////////////////

    // 新建一个object
    GameObject* newObject(ObjectLayer layer, const Vec2& startPos);

    void removeObject(GameObject* ob);

    Quad<GameObject*>& getGameObjects() { return this->_quad_tree; }

    void mainUpdateLogic();

    void mainUpdateCamera();

    Node* getGameMapTarget() { return _game_map_target; }

    Node* getGameRendererTarget() { return _game_renderer_target; }

    Node* getGameBkTarget() { return _game_bk_target; }

    // 摄像机跟随
    void setCameraFollow(GameObject* object) { _camera_follow_object = object; }

    shared_ptr<Random> getGlobalRandom() { return this->_global_random; }

private:
    void mainUpdateInScreenRect(const Vec2& left_bottom, const Size& size);

    void processContact(PhysicsContact& conatct);
    void updateGameObjectPosition();

private:

    set<GameObject*> _need_to_remove;
    map < GameObject*, pair<ObjectLayer,Vec2>> _need_to_add;

    map<string, GameObject*> _game_objects;

    // 摄像机的中心点，坐标为地图中坐标
    Vec2 _camera_pos;
    // 摄像机的目标坐标，实现缓动效果
    Vec2 _camera_pos_target;

    GameObject* _camera_follow_object = nullptr;

    Node* _game_map_target;
    Node* _game_renderer_target;
    Node* _game_node;
    Node* _game_bk_target;
    Quad<GameObject*> _quad_tree;
    shared_ptr<GameMap> _game_map;
    shared_ptr<GameWorldRenderer> _game_renderer;

    queue<GameAct> _game_act_que;

    // 游戏内的任何粒子创建，随机等，都必须用此随机引擎，保证一致性
    shared_ptr<Random> _global_random;
};

class GameWorldRenderer {
public:
    virtual void init(Node* target) = 0;
    virtual void release() = 0;
    virtual void update(const Vec2& left_bottom, const Size& size,
                        GameWorld* gameworld) = 0;
    virtual Vec2 calcuCameraSpeed(const Vec2& current_pos,
                                    const Vec2& target_pos) = 0;
};

#endif