#include "scene/StartScene.h"

#include <sstream>

#include "RefLineLayer.h"
#include "SimpleAudioEngine.h"
#include "audio/include/AudioEngine.h"
#include "game/game_frame/GameFrame.h"
#include "game/game_object/GameObject.h"
#include "scene/GameScene.h"
#include "ui/CocosGUI.h"
#include "utility/PhysicsShapeCache.h"
#include "utility/json/json.h"

using namespace cocos2d::experimental;

USING_NS_CC;

cocos2d::Scene* TestScene::createScene() { return TestScene::create(); }

bool TestScene::init() {
    if (!Scene::init()) {
        return false;
    }
    return true;
}

Scene* DemoScene::createScene() { return DemoScene::create(); }

bool DemoScene::init() {
    if (!Scene::init()) {
        return false;
    }

    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("demo-1.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(
        "loadingPage.plist");

    PhysicsShapeCache::getInstance()->addShapesWithFile("demo_physics.plist");

    auto visibleSize = Director::getInstance()->getVisibleSize();

    this->scheduleOnce(
        [&](float) {
            auto s = GameScene::createScene();
            Director::getInstance()->replaceScene(s);

            int seed = 123;
            int player_cnt = 1;
            vector<string> player_uid = {"abcdef"};
            s->start(seed, player_cnt, player_uid);
        },
        0.1f, "123");

    return true;
}