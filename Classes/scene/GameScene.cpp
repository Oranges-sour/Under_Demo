#include "GameScene.h"

#include "game/game_frame/GameFrame.h"
#include "game/game_map/implements/MapDecorationCreator1.h"
#include "game/game_map/implements/MapGenerator1.h"
#include "game/game_map/implements/MapHelper1.h"
#include "game/game_map/implements/MapPhysics1.h"
#include "game/game_map/implements/MapPreRenderer1.h"
#include "game/game_object/implements/player/player_1/Player1.h"
#include "game/game_world/GameWorld.h"
#include "game/game_world/implements/WorldRenderer1.h"
#include "scene/StartScene.h"
#include "utility/PhysicsShapeCache.h"
#include "utility/touch/Joystick.h"
#include "utility/touch/TouchesPool.h"

GameScene* GameScene::createScene() { return GameScene::create(); }

bool GameScene::init() {
    if (!Scene::initWithPhysics()) {
        return true;
    }

    auto phyw = this->getPhysicsWorld();
    phyw->setGravity(Vec2::ZERO);
    // phyw->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    phyw->setUpdateRate(2);

    loading_layer = LoadingLayer::create();
    this->addChild(loading_layer, 1);

    this->schedule([&](float dt) { Connection::instance()->update(dt * 1000); },
                   "web_upd");

    auto listener = make_shared<ConnectionEventListener>(
        [&](const json& event) { notice(event); });
    Connection::instance()->regeist_event_listener(listener,
                                                   "GameScene_listener");

    return true;
}

void GameScene::start(unsigned seed, int player_cnt,
                      const vector<string>& player_uid) {
    this->player_uid = player_uid;
    this->seed = seed;
    init_map(seed);
}

void GameScene::init_map(unsigned seed) {
    // 创建地图
    this->game_map = make_shared<GameMap>(256, 256);

    // 创建地图生成器
    auto map_generator = make_shared<MapGenerator1>();
    map_generator->init(seed);
    // auto map_generator = make_shared<MapGenerator2>();

    auto map_helper = make_shared<MapHelper1>();
    auto map_physics = make_shared<MapPhysics1>();

    game_map->init(map_generator, map_helper, map_physics);

    this->game_map_pre_renderer = make_shared<MapPreRenderer1>();
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

    this->_frame_manager = make_shared<GameFrameManager>();
    _frame_manager->init(game_world);
    this->schedule([&](float) { _frame_manager->update(); },
                   "frame_manager_upd");

    game_map_pre_renderer->afterPreRender(game_world->get_game_map_target());
   

    game_world->setGameMap(game_map);

    auto ren = make_shared<GameWorldRenderer1>();
    game_world->setGameRenderer(ren);
    ren->init(game_world->get_game_renderer_target());

    auto dec = make_shared<MapDecorationCreator1>();
    dec->setDec2Pos(game_map_pre_renderer->getDec2Pos());
    dec->generate(game_world, &game_map->get(), seed);

    
     // 释放
    game_map_pre_renderer.reset();

    auto game_bk = Sprite::create("game_bk.png");
    game_bk->setAnchorPoint(Vec2(0, 0));
    game_bk->setPosition(Vec2(0, 0));
    Texture2D::TexParams texParams = {GL_LINEAR, GL_LINEAR, GL_REPEAT,
                                      GL_REPEAT};
    game_bk->getTexture()->setTexParameters(texParams);
    game_bk->setTextureRect(Rect(0, 0, 64 * 256, 64 * 256));
    game_world->get_game_bk_target()->addChild(game_bk);

    // 创建玩家
    for (auto& it : player_uid) {
        auto ob = Player1::create(game_world, Vec2(300, 300), it);

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

    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseDown = [&](EventMouse* eventMouse) {
        auto b = eventMouse->getMouseButton();
        auto pos = eventMouse->getLocationInView();
        if (b == EventMouse::MouseButton::BUTTON_LEFT) {
            GameAct act;
            act.type = act_attack;
            act.uid = players.find(Connection::instance()->get_uid())
                          ->second->getUID();

            act.param1 = 1;
            act.param2 = 0;
            _frame_manager->pushGameAct(act);
        }
    };

    mouseListener->onMouseUp = [&](EventMouse* eventMouse) {};

    auto touchListener = EventListenerTouchAllAtOnce::create();
    touchListener->onTouchesBegan = [&](vector<Touch*> touch, Event*) -> void {
        for (auto& t : touch) {
            TouchesPool::instance->addToPool(t);
        }
    };

    touchListener->onTouchesEnded = [&](vector<Touch*> touch, Event*) -> void {
        for (auto& t : touch) {
            TouchesPool::instance->removeFromPool(t);
        }
    };

    auto _dis = Director::getInstance()->getEventDispatcher();
    _dis->addEventListenerWithSceneGraphPriority(keyboardListener, this);
    _dis->addEventListenerWithSceneGraphPriority(mouseListener, this);
    _dis->addEventListenerWithSceneGraphPriority(touchListener, this);

    this->joystick_move =
        Joystick::create("joystick_large.png", "joystick_small.png",
                         "joystick_large_effect.png");
    joystick_move->setOriginalPosition(Vec2(300, 300));
    this->addChild(joystick_move, 1);

    this->joystick_attack =
        Joystick::create("joystick_attack_bk.png", "joystick_attack.png",
                         "joystick_attack_bk_effect.png");
    joystick_attack->setOriginalPosition(Vec2(1620, 300));
    this->addChild(joystick_attack, 1);

    this->schedule(
        [&](float) {
            static stack<Vec2> st;

            static bool can_jump = true;

            while (!st.empty()) {
                auto vec = st.top();
                st.pop();

                GameAct act;
                act.type = act_move_stop;
                act.uid = Connection::instance()->get_uid();
                act.param1 = vec.x;
                _frame_manager->pushGameAct(act);
            }

            auto vec = joystick_move->getMoveVec();
            if (vec == Vec2::ZERO) {
                return;
            }
            GameAct act;
            act.type = act_move_start;
            act.uid = Connection::instance()->get_uid();
            act.param1 = vec.x;
            _frame_manager->pushGameAct(act);

            st.push(vec);

            if (vec.y < 0.7) {
                can_jump = true;
            }

            if (vec.y > 0.7 && can_jump) {
                can_jump = false;
                GameAct act;
                act.type = act_jump;
                act.uid = Connection::instance()->get_uid();
                // act.param1 = vec.x;
                _frame_manager->pushGameAct(act);
            }
        },
        0.0333f, "move_upd");

    this->schedule(
        [&](float) {
            for (auto& it : players) {
                auto ob = it.second;
                if (ob->getUID() == Connection::instance()->get_uid()) {
                    GameAct act;
                    act.type = act_position_force_set;
                    act.uid = ob->getUID();
                    act.param1 = ob->getPosition().x;
                    act.param2 = ob->getPosition().y;

                    _frame_manager->pushGameAct(act, false);
                }
            }
        },
        0.03f, "position_force_set");

    this->schedule(
        [&](float) {
            auto vec = joystick_attack->getMoveVec();
            if (vec == Vec2::ZERO) {
                return;
            }

            GameAct act;
            act.type = act_attack;
            act.uid = Connection::instance()->get_uid();

            act.param1 = vec.x;
            act.param2 = vec.y;
            _frame_manager->pushGameAct(act);
        },
        0.4f, "attack_upd");

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
    GameAct act;
    act.type = act_move_start;
    act.uid = Connection::instance()->get_uid();

    switch (key) {
        case EventKeyboard::KeyCode::KEY_W: {
            GameAct act;
            act.type = act_jump;
            act.uid = Connection::instance()->get_uid();
            _frame_manager->pushGameAct(act);
        } break;
        case EventKeyboard::KeyCode::KEY_S: {
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

    _frame_manager->pushGameAct(act);
}

void GameScene::keyUp(EventKeyboard::KeyCode key) {
    GameAct act;
    act.type = act_move_stop;
    act.uid = Connection::instance()->get_uid();

    switch (key) {
        case EventKeyboard::KeyCode::KEY_W: {
        } break;
        case EventKeyboard::KeyCode::KEY_S: {
        } break;
        case EventKeyboard::KeyCode::KEY_A: {
            act.param1 = -1;
        } break;
        case EventKeyboard::KeyCode::KEY_D: {
            act.param1 = 1;
        } break;
    }

    _frame_manager->pushGameAct(act);
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
