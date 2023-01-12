#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include <string>
using namespace std;

#include "cocos/network/WebSocket.h"
#include "cocos2d.h"
using namespace cocos2d;
using namespace cocos2d::network;

#include "Connection.h"
#include "GameComponet.h"
#include "GameFrame.h"
#include "Random.h"
class GameWorld;

#include <chrono>
using namespace std::chrono;

class GameMap;
class MapPreRendererComponent1;

class LoadingLayer;

class GameScene : public Scene {
public:
    static GameScene* createScene();

    virtual bool init() override;

    void set_connection(shared_ptr<Connection> connection) {
        this->schedule([&](float) { this->connection->update(25); }, 0.025,
                       "update_con");
        this->connection = connection;

        auto listener = make_shared<ConnectionEventListener>(
            [&](const json& event) { notice(event); });
        connection->regeist_event_listener(listener, "GameScene_listener");
    }

    void start(unsigned seed, int player_cnt, const vector<string>& player_uid);

    void init_map(unsigned seed);

    void init_game();

    void notice(const json& event);

    virtual void cleanup() override {
        connection->remove_event_listener("GameScene_listener");
        Scene::cleanup();
    }

    CREATE_FUNC(GameScene);

private:
    void keyDown(EventKeyboard::KeyCode key);
    void keyUp(EventKeyboard::KeyCode key);

private:
    int player_cnt;
    vector<string> player_uid;

    map<string, GameObject*> players;

    GameWorld* game_world;
    shared_ptr<GameMap> game_map;
    shared_ptr<MapPreRendererComponent1> game_map_pre_renderer;

    shared_ptr<Connection> connection;

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

class PhysicsComponent1 : public PhysicsComponent {
public:
    PhysicsComponent1(const Vec2& pos) { this->posNow = pos; }

    virtual void receiveGameAct(GameObject* ob, const GameAct& act) override {}
    virtual void receiveEvent(GameObject* ob, const json& event) override;
};

class TestAi : public GameComponent {
public:
    TestAi() {}
    virtual void updateLogic(GameObject* ob) override;
    virtual void updateDraw(GameObject* ob, float rate) override {}
    virtual void receiveGameAct(GameObject* ob, const GameAct& event) override;
    virtual void receiveEvent(GameObject* ob, const json& event) override {}

private:
    int xx = 0;
    int yy = 0;
};

#endif