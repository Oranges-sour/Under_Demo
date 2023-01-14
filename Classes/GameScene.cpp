#include "GameScene.h"

#include "GameComponet.h"
#include "GameFrame.h"
#include "GameMap.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "HelloWorldScene.h"
#include "PhysicsShapeCache.h"

steady_clock::time_point time_0;
steady_clock::time_point time_1;

GameScene* GameScene::createScene() { return GameScene::create(); }

bool GameScene::init() {
    if (!Scene::initWithPhysics()) {
        return true;
    }

    auto phyw = this->getPhysicsWorld();
    phyw->setGravity(Vec2::ZERO);
    phyw->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
   // phyw->setUpdateRate(0.5);

    loading_layer = LoadingLayer::create();
    this->addChild(loading_layer, 1);

    this->schedule([&](float) { Connection::instance()->update(16); },
                   "web_upd");

    auto listener = make_shared<ConnectionEventListener>(
        [&](const json& event) { notice(event); });
    Connection::instance()->regeist_event_listener(listener,
                                                   "GameScene_listener");

    return true;
}

void GameScene::start(unsigned seed, int player_cnt,
                      const vector<string>& player_uid) {
    this->player_cnt = player_cnt;
    this->player_uid = player_uid;
    init_map(seed);
}

void GameScene::init_map(unsigned seed) {
    // 创建地图
    this->game_map = make_shared<GameMap>(256, 256);

    auto map_generator = make_shared<MapGeneratorComponent1>();
    // auto map_generator = make_shared<MapGeneratorComponent2>();
    map_generator->init(seed);

    auto map_helper = make_shared<MapHelperComponent1>();
    auto map_physics = make_shared<MapPhysicsComponent1>();

    game_map->init(map_generator, map_helper, map_physics);

    this->game_map_pre_renderer = make_shared<MapPreRendererComponent1>();
    game_map_pre_renderer->init(&game_map->get(), seed);

    this->schedule(
        [&](float) {
            if (game_map_pre_renderer->isPreRenderFinish()) {
                this->unschedule("pre_render");

                loading_layer->removeFromParent();
                
                init_game();
                return;
            }
            // x2加速渲染
            game_map_pre_renderer->preRender();
            game_map_pre_renderer->preRender();
        },
        "pre_render");
}

void GameScene::init_game() {
    this->game_world = GameWorld::create();
    this->addChild(game_world);

    game_map_pre_renderer->afterPreRender(game_world->get_game_map_target());
    //释放
    game_map_pre_renderer.reset();

    game_world->setGameMap(game_map);

    auto ren = make_shared<GameWorldRenderer1>();
    game_world->setGameRenderer(ren);
    ren->init(game_world->get_game_renderer_atrget());

    auto frame = make_shared<GameFrameManager>();
    game_world->setGameFrameManager(frame);

    for (auto& it : player_uid) {
        ///
        auto ob = game_world->newObject(1, Vec2(300, 300));
        ob->initWithSpriteFrameName("enemy_0.png");
        ob->setPosition(300, 300);
        auto phy = make_shared<PhysicsComponent1>(Vec2(300, 300));
        ob->addGameComponent(phy);

        auto ai = make_shared<TestAi>();
        ob->addGameComponent(ai);

        ob->setGameObjectType(object_type_player);

        ob->setLightRadius(600);
        ob->setLightColor(Color3B(255, 255, 255));

        PhysicsShapeCache::getInstance()->setBodyOnSprite("enemy_0", ob);

        players.insert({it, ob});
        if (it == Connection::instance()->get_uid()) {
            game_world->camera_follow(ob);
        }
    }

    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = [&](EventKeyboard::KeyCode key,
                                         Event* en) { keyDown(key); };

    keyboardListener->onKeyReleased = [&](EventKeyboard::KeyCode key,
                                          Event* en) { keyUp(key); };
    auto _dis = Director::getInstance()->getEventDispatcher();
    _dis->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    // 初始化完毕，要手动给一帧
    json js = frame->generateJsonOfNextFrame(Connection::instance()->get_uid());
    frame->newNextFrame();
    Connection::instance()->push_statueEvent(js);
}

void GameScene::notice(const json& event) {
    if (!event.contains("type")) {
        return;
    }

    string type = event["type"];
    if (type == "quit_game_result") {
        string statue = event["statue"];
        if (statue == "success") {
            auto s = DemoScene::createScene();
            Director::getInstance()->replaceScene(s);
            return;
        }
    }
}

void GameScene::keyDown(EventKeyboard::KeyCode key) {
    auto frame_man = game_world->getGameFrameManager();
    GameAct act;
    act.type = act_move_start;
    act.uid = players.find(Connection::instance()->get_uid())->second->getUID();

    switch (key) {
        case EventKeyboard::KeyCode::KEY_W: {
            act.param1 = 0;
            act.param2 = 1;
        } break;
        case EventKeyboard::KeyCode::KEY_S: {
            act.param1 = 0;
            act.param2 = -1;
        } break;
        case EventKeyboard::KeyCode::KEY_A: {
            act.param1 = -1;
            act.param2 = 0;
        } break;
        case EventKeyboard::KeyCode::KEY_D: {
            act.param1 = 1;
            act.param2 = 0;
        } break;
    }

    frame_man->pushGameAct(act);
}

void GameScene::keyUp(EventKeyboard::KeyCode key) {
    auto frame_man = game_world->getGameFrameManager();
    GameAct act;
    act.type = act_move_stop;
    act.uid = players.find(Connection::instance()->get_uid())->second->getUID();

    switch (key) {
        case EventKeyboard::KeyCode::KEY_W: {
            act.param1 = 0;
            act.param2 = 1;
        } break;
        case EventKeyboard::KeyCode::KEY_S: {
            act.param1 = 0;
            act.param2 = -1;
        } break;
        case EventKeyboard::KeyCode::KEY_A: {
            act.param1 = -1;
            act.param2 = 0;
        } break;
        case EventKeyboard::KeyCode::KEY_D: {
            act.param1 = 1;
            act.param2 = 0;
        } break;
    }

    frame_man->pushGameAct(act);
}

bool LoadingLayer::init() {
    if (!Layer::init()) {
        return false;
    }

    const auto visibleSize = Director::getInstance()->getVisibleSize();

    this->bk = Sprite::create("blue_bk.png");
    bk->setPosition(visibleSize / 2);
    this->addChild(bk, 2);

    this->spAct0 = Sprite::createWithSpriteFrameName("mapDrawWaitAct0.png");
    spAct0->setPosition(visibleSize / 2);
    this->addChild(spAct0, 3);

    for (int x = 0; x < 9; ++x) {
        auto sp = Sprite::createWithSpriteFrameName("mapDrawWaitAct1.png");
        loadingBlocks[x] = sp;
        auto pos = getLoadingBlocksPos(x, 1);
        sp->setPosition(pos);
        sp->setScale(0);
        this->addChild(sp, 3);
    }

    this->schedule(
        [&](float) {
            static int x = 0;

            auto sp = loadingBlocks[x];

            auto a0 = ScaleTo::create(0.3f, 1.2f);
            auto e0 = EaseIn::create(a0, 2.0f);
            auto a1 = ScaleTo::create(0.15f, 1.0f);
            auto e1 = EaseOut::create(a1, 2.0f);

            auto a2 = ScaleTo::create(0.3f, 0.0f);
            auto e2 = EaseIn::create(a2, 2.0f);

            auto a3 = DelayTime::create(0.8f);

            auto s0 = Sequence::create({e0, e1, a3, e2});
            sp->runAction(RepeatForever::create(s0));
            ++x;
            if (x == 9) {
                this->unschedule("startBlocks");
            }
        },
        0.1f, "startBlocks");

    return true;
}

void TestAi::updateLogic(GameObject* ob) {
    json event;
    event["type"] = "move";
    event["x"] = xx;
    event["y"] = yy;

    ob->pushEvent(event);
}

void TestAi::receiveGameAct(GameObject* ob, const GameAct& event) {
    if (event.type == act_move_start) {
        if (ob->getUID() == event.uid) {
            xx += event.param1 * 30;
            yy += event.param2 * 30;
        }
    }
    if (event.type == act_move_stop) {
        if (ob->getUID() == event.uid) {
            xx -= event.param1 * 30;
            yy -= event.param2 * 30;
        }
    }
}

void PhysicsComponent1::receiveEvent(GameObject* ob, const json& event) {
    string type = event["type"];
    if (type == "move") {
        float x = event["x"];
        float y = event["y"];
        if (abs(y - 10) < 0.01) {
            time_1 = steady_clock::now();
            auto d = duration_cast<duration<float>>(time_1 - time_0);
            CCLOG("%f", d.count());
        }

        posNow += Vec2(x, y);
        return;
    }
}

// void PhysicsComponent1::receiveEvent(GameObject* ob, const json& event) {
//     string type = event["type"];
//     if (type == "move") {
//         float x = event["x"];
//         float y = event["y"];
//         if (abs(y - 10) < 0.01) {
//             time_1 = steady_clock::now();
//             auto d = duration_cast<duration<float>>(time_1 - time_0);
//             CCLOG("%f", d.count());
//         }
//
//         posNow += Vec2(x, y);
//         return;
//     }
//     if (type == "contact") {
//         /*long long data = event["object"];
//         GameObject* tar = (GameObject*)data;
//         auto t = tar->getGameObjectType();
//
//         ob->setVisible(false);
//
//         ob->removeFromParent();*/
//     }
// }
