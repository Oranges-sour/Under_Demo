#include "GameScene.h"

#include "RefLineLayer.h"
#include "game/game_frame/GameFrame.h"
#include "game/game_map/implements/MapDecorationCreator1.h"
#include "game/game_map/implements/MapGenerator1.h"
#include "game/game_map/implements/MapHelper1.h"
#include "game/game_map/implements/MapPhysics1.h"
#include "game/game_map/implements/MapPreRenderer1.h"
#include "game/game_object/implements/enemy/enemy_1/Enemy1.h"
#include "game/game_object/implements/other/start_point/StartPoint.h"
#include "game/game_object/implements/player/player_1/Player1.h"
#include "game/game_world/GameWorld.h"
#include "game/game_world/implements/WorldRenderer1.h"
#include "scene/StartScene.h"
#include "utility/GameObjectInfo.h"
#include "utility/PhysicsShapeCache.h"
#include "utility/touch/Joystick.h"
#include "utility/touch/TouchesPool.h"

GameScene* GameScene::createScene() { return GameScene::create(); }

bool GameScene::init() {
    if (!Scene::initWithPhysics()) {
        return true;
    }

    auto vs = Director::getInstance()->getVisibleSize();

    auto phyw = this->getPhysicsWorld();
    phyw->setGravity(Vec2::ZERO);
    // phyw->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    phyw->setUpdateRate(2);

    loading_layer = LoadingLayer::create();
    this->addChild(loading_layer, 1);

    this->schedule([&](float dt) { Connection::instance()->update(dt * 1000); },
                   "web_upd");

    this->schedule([&](float dt) { try_ping(); }, 0.1f, "try_ping");

    auto ping_1 =
        Label::createWithTTF("", "font_normal.otf", 48, Size(300, 80),
                             TextHAlignment::RIGHT, TextVAlignment::TOP);
    ping_1->setAnchorPoint(Vec2(1, 1));
    ping_1->setPosition(vs.width - 100, vs.height - 50);
    this->addChild(ping_1, 2);

    auto ping_2 =
        Label::createWithTTF("", "font_normal.otf", 48, Size(500, 80),
                             TextHAlignment::RIGHT, TextVAlignment::TOP);
    ping_2->setAnchorPoint(Vec2(1, 1));
    ping_2->setPosition(vs.width - 100, vs.height - 120);
    this->addChild(ping_2, 2);

    this->schedule(
        [&, ping_1, ping_2](float dt) {
            ping_que.push_back(ping_ms);
            while (ping_que.size() > 10) {
                ping_que.pop_front();
            }

            {
                stringstream ss;
                ss << "ping: ";
                ss << ping_ms;
                ss << "ms";

                ping_1->setString(ss.str());
            }
            int cnt = 0;
            int ping_sum = 0;
            for (auto& it : ping_que) {
                ping_sum += it;
                cnt += 1;
            }
            if (cnt == 0) {
                return;
            }
            {
                stringstream ss;
                ss << "ping avg: ";
                ss << ping_sum / cnt;
                ss << "ms";

                ping_2->setString(ss.str());
            }
        },
        0.1f, "ping_l");

    auto listener = make_shared<ConnectionEventListener>(
        [&](const json& event) { notice(event); });
    Connection::instance()->addEventListener(listener, "GameScene_listener");

    auto refline = RefLineLayer::create();
    this->addChild(refline, 1000);

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
            // game_map_pre_renderer->preRender();
            game_map_pre_renderer->preRender();
        },
        "pre_render");
}

void GameScene::init_game() {
    auto& info = GameObjectInfo::instance()->get("game_defaults");
    string game_background = info["game_background"];
    vector<string> res_joystick_move = info["joystick_move"];
    float joystick_move_left = info["joystick_move_left"];
    float joystick_move_bottom = info["joystick_move_bottom"];
    vector<string> res_joysitck_attack = info["joysitck_attack"];
    float joystick_attack_right = info["joystick_attack_right"];
    float joystick_attack_bottom = info["joystick_attack_bottom"];

    this->game_world = GameWorld::create();
    this->addChild(game_world);

    this->_frame_manager = make_shared<GameFrameManager>();
    _frame_manager->init(game_world);
    this->schedule([&](float) { _frame_manager->update(); },
                   "frame_manager_upd");

    game_map_pre_renderer->afterPreRender(game_world->getGameMapTarget());

    game_world->setGameMap(game_map);

    auto ren = make_shared<GameWorldRenderer1>();
    game_world->setGameRenderer(ren);
    ren->init(game_world->getGameRendererTarget());

    auto dec = make_shared<MapDecorationCreator1>();
    dec->setDec2Pos(game_map_pre_renderer->getDec2Pos());
    dec->generate(game_world, &game_map->get(), seed);

    // 释放
    game_map_pre_renderer.reset();

    auto game_bk = Sprite::create(game_background);
    game_bk->setAnchorPoint(Vec2(0, 0));
    game_bk->setPosition(Vec2(0, 0));
    Texture2D::TexParams texParams = {GL_LINEAR, GL_LINEAR, GL_REPEAT,
                                      GL_REPEAT};
    game_bk->getTexture()->setTexParameters(texParams);
    game_bk->setTextureRect(Rect(0, 0, 64 * 256, 64 * 256));
    game_world->getGameBkTarget()->addChild(game_bk);

    // 创建玩家
    for (auto& it : player_uid) {
        auto ob =
            Player1::create(game_world, "player_1", Vec2(3 * 64, 253 * 64), it);

        players.insert({it, ob});
        if (it == Connection::instance()->getUID()) {
            game_world->setCameraFollow(ob);
        }
    }

    //创建一个敌人
    Enemy1::create(game_world, "enemy_1", Vec2(4 * 64, 251 * 64));

    // 创建出生点
    StartPoint::create(game_world, "start_point", Vec2(2 * 64, 248 * 64));

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
            /* GameAct act;
             act.type = act_attack;
             act._uid = players.find(Connection::instance()->getUID())
                           ->second->getUID();

             act.param1 = 1;
             act.param2 = 0;
             _frame_manager->pushGameAct(act);*/
        }
    };

    mouseListener->onMouseUp = [&](EventMouse* eventMouse) {};

    auto touchListener = EventListenerTouchAllAtOnce::create();
    touchListener->onTouchesBegan = [&](vector<Touch*> touch, Event*) -> void {
        for (auto& t : touch) {
            TouchesPool::_instance->addToPool(t);
        }
    };

    touchListener->onTouchesEnded = [&](vector<Touch*> touch, Event*) -> void {
        for (auto& t : touch) {
            TouchesPool::_instance->removeFromPool(t);
        }
    };

    auto _dis = Director::getInstance()->getEventDispatcher();
    _dis->addEventListenerWithSceneGraphPriority(keyboardListener, this);
    _dis->addEventListenerWithSceneGraphPriority(mouseListener, this);
    _dis->addEventListenerWithSceneGraphPriority(touchListener, this);

    auto visible_size = Director::getInstance()->getVisibleSize();

    this->joystick_move = Joystick::create(
        res_joystick_move[0], res_joystick_move[1], res_joystick_move[2]);

    this->joystick_move->setOriginalPosition(
        Vec2(joystick_move_left, joystick_move_bottom));
    this->addChild(this->joystick_move, 1);

    this->joystick_attack = Joystick::create(
        res_joysitck_attack[0], res_joysitck_attack[1], res_joysitck_attack[2]);
    joystick_attack->setOriginalPosition(Vec2(
        visible_size.width - joystick_attack_right, joystick_attack_bottom));
    this->addChild(joystick_attack, 1);

    this->schedule(
        [&](float) {
            move_upd();
            jump_upd();
            attack_upd();
        },
        0.03f, "control_upd");

    this->schedule(
        [&](float) {
            for (auto& it : players) {
                auto ob = it.second;
                if (ob->getUID() == Connection::instance()->getUID()) {
                    GameAct act;
                    act.type = act_position_force_set;
                    act.uid = ob->getUID();
                    act.param1 = ob->getPosition().x;
                    act.param2 = ob->getPosition().y;

                    _frame_manager->pushGameAct(act, false);
                }
            }
        },
        0.1f, "position_force_set");

    this->schedule(
        [&](float) {
            /*auto vec = joystick_attack->getMoveVec();
            if (vec == Vec2::ZERO) {
                return;
            }

            GameAct act;
            act.type = act_attack;
            act._uid = Connection::instance()->getUID();

            act.param1 = vec.x;
            act.param2 = vec.y;
            _frame_manager->pushGameAct(act);*/
        },
        0.4f, "attack_upd");

    auto tex = Director::getInstance()->getTextureCache();
    auto tex_info = tex->getCachedTextureInfo();
    CCLOG("%s", tex_info.c_str());
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

    if (type == "ping") {
        ping_time1 = steady_clock::now();
        ping_finish = true;

        ping_ms =
            duration_cast<duration<float>>(ping_time1 - ping_time0).count() *
            1000 / 2;
    }
}

void GameScene::move_upd() {
    const auto stop_move = [this](int x) {
        GameAct act;
        act.type = act_move_stop;
        act.uid = Connection::instance()->getUID();
        act.param1 = x;
        _frame_manager->pushGameAct(act);
    };

    const auto start_move = [this](int x) {
        GameAct act;
        act.type = act_move_start;
        act.uid = Connection::instance()->getUID();
        act.param1 = x;
        _frame_manager->pushGameAct(act);
    };

    static struct {
        void reset() {
            on_move = false;
            x = 0;
        }

        bool on_move = false;
        int x = 0;
    } last_statue;

    auto vec = joystick_move->getMoveVec();

    if (abs(vec.x) < 0.01f) {
        if (last_statue.on_move) {
            stop_move(last_statue.x);

            last_statue.reset();
        }
        return;
    }

    int nx = -1;
    if (vec.x < 0) {
        nx = -1;
    }
    if (vec.x > 0) {
        nx = 1;
    }

    if (last_statue.on_move) {
        // 两次方向一致，说明移动方向一样，不用管
        if (last_statue.x == nx) {
            return;
        }
        // 两次方向不一致，先停掉上一次，再开始这一次
        if (last_statue.x != nx) {
            stop_move(last_statue.x);
            start_move(nx);
            last_statue.x = nx;
        }
    } else {
        start_move(nx);
        last_statue.on_move = true;
        last_statue.x = nx;
    }
}

void GameScene::jump_upd() {
    if (!_can_jump) {
        return;
    }
    auto vec = joystick_move->getMoveVec();
    if (vec.y > 0.5) {
        _can_jump = false;
        this->schedule([&](float) { _can_jump = true; }, 0.7f, "reset_jump");

        GameAct act;
        act.type = act_jump;
        act.uid = Connection::instance()->getUID();
        _frame_manager->pushGameAct(act);
    }
}

void GameScene::attack_upd() {
    const auto stop_attack = [this](int x) {
        GameAct act;
        act.type = act_attack_stop;
        act.uid = Connection::instance()->getUID();
        act.param1 = x;
        _frame_manager->pushGameAct(act);
    };

    const auto start_attack = [this](int x) {
        GameAct act;
        act.type = act_attack_start;
        act.uid = Connection::instance()->getUID();
        act.param1 = x;
        _frame_manager->pushGameAct(act);
    };

    static struct {
        void reset() {
            on_attack = false;
            x = 0;
        }

        bool on_attack = false;
        int x = 0;
    } last_statue;

    auto vec = joystick_attack->getMoveVec();

    if (abs(vec.x) < 0.01f) {
        if (last_statue.on_attack) {
            stop_attack(last_statue.x);

            last_statue.reset();
        }
        return;
    }

    int nx = -1;
    if (vec.x < 0) {
        nx = -1;
    }
    if (vec.x > 0) {
        nx = 1;
    }

    if (last_statue.on_attack) {
        // 两次方向一致，说明移动方向一样，不用管
        if (last_statue.x == nx) {
            return;
        }
        // 两次方向不一致，先停掉上一次，再开始这一次
        if (last_statue.x != nx) {
            stop_attack(last_statue.x);
            start_attack(nx);
            last_statue.x = nx;
        }
    } else {
        start_attack(nx);
        last_statue.on_attack = true;
        last_statue.x = nx;
    }
}

void GameScene::keyDown(EventKeyboard::KeyCode key) {
    switch (key) {
        case EventKeyboard::KeyCode::KEY_W: {
            GameAct act;
            act.type = act_jump;
            act.uid = Connection::instance()->getUID();
            _frame_manager->pushGameAct(act);
        } break;
        case EventKeyboard::KeyCode::KEY_S: {
        } break;
        case EventKeyboard::KeyCode::KEY_A: {
            GameAct act;
            act.type = act_move_start;
            act.uid = Connection::instance()->getUID();
            act.param1 = -1;
            _frame_manager->pushGameAct(act);

        } break;
        case EventKeyboard::KeyCode::KEY_D: {
            GameAct act;
            act.type = act_move_start;
            act.uid = Connection::instance()->getUID();
            act.param1 = 1;
            _frame_manager->pushGameAct(act);
        } break;
    }
}

void GameScene::keyUp(EventKeyboard::KeyCode key) {
    switch (key) {
        case EventKeyboard::KeyCode::KEY_W: {
        } break;
        case EventKeyboard::KeyCode::KEY_S: {
        } break;
        case EventKeyboard::KeyCode::KEY_A: {
            GameAct act;
            act.type = act_move_stop;
            act.uid = Connection::instance()->getUID();
            act.param1 = -1;
            _frame_manager->pushGameAct(act);
        } break;
        case EventKeyboard::KeyCode::KEY_D: {
            GameAct act;
            act.type = act_move_stop;
            act.uid = Connection::instance()->getUID();
            act.param1 = 1;
            _frame_manager->pushGameAct(act);
        } break;
    }
}

void GameScene::try_ping() {
    if (!ping_finish) {
        return;
    }

    json e;
    e["type"] = "ping";
    Connection::instance()->pushStatueEvent(e);
    ping_time0 = steady_clock::now();
    ping_finish = false;
}

bool LoadingLayer::init() {
    if (!Layer::init()) {
        return false;
    }

    auto& info = GameObjectInfo::instance()->get("loading_layer");
    string res_bk = info["bk"];
    string out_line = info["out_line"];
    string blocks = info["blocks"];

    const auto visibleSize = Director::getInstance()->getVisibleSize();

    this->bk = Sprite::create(res_bk);
    bk->setPosition(visibleSize / 2);
    this->addChild(bk, 2);

    this->spAct0 = Sprite::createWithSpriteFrameName(out_line);
    spAct0->setPosition(visibleSize / 2);
    this->addChild(spAct0, 3);

    for (int x = 0; x < 9; ++x) {
        auto sp = Sprite::createWithSpriteFrameName(blocks);
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
