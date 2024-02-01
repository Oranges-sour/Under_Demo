#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include <string>
using namespace std;

//#include "cocos/network/WebSocket.h"
#include "cocos2d.h"
using namespace cocos2d;
//using namespace cocos2d::network;

//#include "web/Connection.h"

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

    // implement the "static create()" method manually
    CREATE_FUNC(DemoScene);

};



#endif  // __HELLOWORLD_SCENE_H__
