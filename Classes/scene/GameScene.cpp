#include "GameScene.h"

#include "RefLineLayer.h"
#include "audio/include/AudioEngine.h"
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
#include "utility/math/MyMath.h"
#include "utility/touch/Joystick.h"
#include "utility/touch/TouchesPool.h"
using namespace cocos2d::experimental;

const string PLAYER_UID = "playeruid";

GameScene* GameScene::createScene() { return GameScene::create(); }

bool GameScene::init() {
    if (!Scene::initWithPhysics()) {
        return true;
    }

    auto vs = Director::getInstance()->getVisibleSize();

    auto phyw = this->getPhysicsWorld();
    phyw->setGravity(Vec2::ZERO);
    // phyw->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    // phyw->setUpdateRate(1);

    loading_layer = LoadingLayer::create();
    this->addChild(loading_layer, 1);

    /*auto refline = RefLineLayer::create();
    this->addChild(refline, 1000);*/

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
            game_map_pre_renderer->preRender();
            game_map_pre_renderer->preRender();
            game_map_pre_renderer->preRender();
            game_map_pre_renderer->preRender();
            game_map_pre_renderer->preRender();
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
        if (it == PLAYER_UID) {
            game_world->setCameraFollow(ob);
        }
    }

    // 创建一个敌人
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
             act._uid = players.find(PLAYER_UID)
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

    // this->attack_draw = DrawNode::create();
    // this->addChild(attack_draw, 1);
    //{
    //     float radius = 600;
    //     // 离中心区域的位置没用线
    //     float radius_1 = 50;
    //     float r = 60;
    //     float offset = 90;
    //     Vec2 direction(DEG::cos(r), DEG::sin(r));
    //     Vec2 direction_1(DEG::cos(r - offset), DEG::sin(r - offset));
    //     Vec2 direction_2(DEG::cos(r + offset), DEG::sin(r + offset));

    //    attack_draw->drawLine(Vec2(visible_size / 2) + direction_1 * radius_1,
    //                          Vec2(visible_size / 2) + direction_1 * radius,
    //                          Color4F(0.6, 0.6, 0.6, 1.0));
    //    attack_draw->drawLine(Vec2(visible_size / 2) + direction_2 * radius_1,
    //                          Vec2(visible_size / 2) + direction_2 * radius,
    //                          Color4F(0.6, 0.6, 0.6, 1.0));

    //    Vec2 vv[500];
    //    int cnt = 0;
    //    for (int i = 0; i < 500; ++i) {
    //        vv[i] = Vec2::ZERO;
    //    }
    //    // 起点
    //    vv[cnt] = Vec2(visible_size / 2) + direction_1 * radius_1;
    //    cnt += 1;
    //    // 大圆弧上
    //    for (int an = r - offset; an <= r + offset; an += 5) {
    //        vv[cnt] = Vec2(visible_size / 2) +
    //                  Vec2(DEG::cos(an), DEG::sin(an)) * radius;
    //        cnt += 1;
    //    }
    //    // 小圆弧上
    //   /* for (int an = r + offset; an > r - offset; an -= 5) {
    //        vv[cnt] = Vec2(visible_size / 2) +
    //                  Vec2(DEG::cos(an), DEG::sin(an)) * radius_1;
    //        cnt += 1;
    //    }*/
    //    // 终点
    //     vv[cnt] = Vec2(visible_size / 2) + direction_2 * (radius_1 + 1);
    //     cnt += 1;
    //    attack_draw->drawSolidPoly(vv, cnt, Color4F(1.0, 1.0, 1.0, 0.1));
    //}

    this->schedule(
        [&](float) {
            move_upd();
            jump_upd();
            attack_upd();
        },
        0.03f, "control_upd");

    this->schedule(
        [&](float) {
            /*auto vec = joystick_attack->getMoveVec();
            if (vec == Vec2::ZERO) {
                return;
            }

            GameAct act;
            act.type = act_attack;
            act._uid = PLAYER_UID;

            act.param1 = vec.x;
            act.param2 = vec.y;
            _frame_manager->pushGameAct(act);*/
        },
        0.4f, "attack_upd");

    auto tex = Director::getInstance()->getTextureCache();
    auto tex_info = tex->getCachedTextureInfo();
    CCLOG("%s", tex_info.c_str());

    // Audio
    AudioEngine::play2d("11.mp3", true, 0.6);
}

void GameScene::move_upd() {
    const auto stop_move = [this](int x) {
        GameEvent event{GameEventType::control_move_stop, "", 0, 0, 0, ""};
        event.param1.f_val = x;

        game_world->pushGameEvent(event, PLAYER_UID);
    };

    const auto start_move = [this](int x) {
        GameEvent event{GameEventType::control_move_start, "", 0, 0, 0, ""};
        event.param1.f_val = x;

        game_world->pushGameEvent(event, PLAYER_UID);
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

        GameEvent event{GameEventType::control_jump, "", 0, 0, 0, ""};
        game_world->pushGameEvent(event, PLAYER_UID);
    }
}

void GameScene::attack_upd() {
    const auto stop_attack = [this](int x) {
        GameEvent event{GameEventType::control_attack_stop, "", 0, 0, 0, ""};
        event.param1.f_val = x;

        game_world->pushGameEvent(event, PLAYER_UID);
    };

    const auto start_attack = [this](int x) {
        GameEvent event{GameEventType::control_attack_start, "", 0, 0, 0, ""};
        event.param1.f_val = x;

        game_world->pushGameEvent(event, PLAYER_UID);
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
            GameEvent event{
                GameEventType::control_jump, "", 0, 0, 0, ""};
            game_world->pushGameEvent(event, PLAYER_UID);
        } break;
        case EventKeyboard::KeyCode::KEY_S: {
        } break;
        case EventKeyboard::KeyCode::KEY_A: {
            GameEvent event{
                GameEventType::control_move_start, "", 0, 0, 0, ""};
            event.param1.f_val = -1;

            game_world->pushGameEvent(event, PLAYER_UID);

        } break;
        case EventKeyboard::KeyCode::KEY_D: {
            GameEvent event{GameEventType::control_move_start, "", 0, 0, 0, ""};
            event.param1.f_val = 1;

            game_world->pushGameEvent(event, PLAYER_UID);
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
            GameEvent event{GameEventType::control_move_stop, "", 0, 0, 0, ""};
            event.param1.f_val = -1;

            game_world->pushGameEvent(event, PLAYER_UID);
        } break;
        case EventKeyboard::KeyCode::KEY_D: {
            GameEvent event{GameEventType::control_move_stop, "", 0, 0, 0, ""};
            event.param1.f_val = 1;

            game_world->pushGameEvent(event, PLAYER_UID);
        } break;
    }
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
