#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include <string>
using namespace std;

#include "cocos2d.h"
using namespace cocos2d;

#include "game/game_frame/GameFrame.h"
#include "game/game_object/GameObject.h"
#include "utility/math/Random.h"
#include "web/Connection.h"

class Joystick;
class GameWorld;

#include <chrono>
using namespace std::chrono;

class GameMap;
class MapPreRenderer1;

class LoadingLayer;

class GameScene : public Scene {
public:
    static GameScene* createScene();

    virtual bool init() override;

    void start(unsigned seed, int player_cnt, const vector<string>& player_uid);

    void init_map(unsigned seed);

    void init_game();

    virtual void cleanup() override {
        Connection::instance()->removeEventListener("GameScene_listener");
        _frame_manager->release();
        Scene::cleanup();
    }

    CREATE_FUNC(GameScene);

private:
    void move_upd();
    void jump_upd();
    void attack_upd();

    bool _can_jump = true;

private:
    void keyDown(EventKeyboard::KeyCode key);
    void keyUp(EventKeyboard::KeyCode key);


private:
    unsigned int seed;

    vector<string> player_uid;

    Joystick* joystick_move;
    Joystick* joystick_attack;
    DrawNode* attack_draw;

    map<string, GameObject*> players;

    GameWorld* game_world;
    shared_ptr<GameMap> game_map;
    shared_ptr<MapPreRenderer1> game_map_pre_renderer;

    shared_ptr<GameFrameManager> _frame_manager;

    LoadingLayer* loading_layer;
};

class LoadingLayer : public Layer {
public:
    virtual bool init();

    CREATE_FUNC(LoadingLayer);

private:
    Vec2 getLoadingBlocksPos(int x, float k) {
        const auto visibleSize = Director::getInstance()->getVisibleSize();

        const float diff = 165;

        std::array<Vec2, 9> arr{Vec2(-1, 1),  Vec2(0, 1),  Vec2(1, 1),
                                Vec2(-1, 0),  Vec2(0, 0),  Vec2(1, 0),
                                Vec2(-1, -1), Vec2(0, -1), Vec2(1, -1)};
        Vec2 v = visibleSize / 2;
        return v + arr[x] * diff / k;
    }
    std::array<Sprite*, 9> loadingBlocks;
    Sprite* bk = nullptr;
    Sprite* spAct0 = nullptr;
};

#endif