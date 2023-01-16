#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include <string>
using namespace std;

#include "cocos/network/WebSocket.h"
#include "cocos2d.h"
using namespace cocos2d;
using namespace cocos2d::network;

#include "Connection.h"

class Lobby_Layer;
class Game_Layer;

class TestScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(TestScene);

};

class DemoScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    void init_after_connect();

    // implement the "static create()" method manually
    CREATE_FUNC(DemoScene);

private:
    Lobby_Layer* lobby = nullptr;
};

class Lobby_Layer : public Layer {
private:
    class GameInfo : public Node {
    public:
        virtual bool init();

        CREATE_FUNC(GameInfo);

        Label* info;
        MenuItemLabel* button;

        string uid;
    };

public:
    virtual bool init();

    virtual void cleanup() override {
        Connection::instance()->remove_event_listener("Lobby_Layer_listener");
        Layer::cleanup();
    }

    void notice(const json& event);

    CREATE_FUNC(Lobby_Layer);

private:
    bool is_in_game = false;
    bool is_host = false;
    Node* not_in_game = nullptr;
    Node* in_game = nullptr;

    Label* my_uid = nullptr;
    vector<GameInfo*> game_info;

    vector<Label*> in_game_message;
};

#endif  // __HELLOWORLD_SCENE_H__
