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

    //
    Connection::instance()->open("ws://127.0.0.1:23482");

    // Connection::instance()->open("ws://127.0.0.1:23482");

    auto visibleSize = Director::getInstance()->getVisibleSize();

    auto la = Label::createWithTTF("Connecting...", "font_normal.otf", 48,
                                   Size(500, 200), TextHAlignment::CENTER,
                                   TextVAlignment::CENTER);
    la->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    this->addChild(la);

    this->schedule(
        [&, la](float) {
            if (Connection::instance()->isError()) {
                Connection::instance()->open("ws://101.42.237.241:23482");
                // Connection::instance()->open("ws://127.0.0.1:23482");
            } else if (Connection::instance()->isOpen()) {
                this->unschedule("check_server_online");
                this->init_after_connect();
                la->setVisible(false);
            }
        },
        "check_server_online");

    auto refline = RefLineLayer::create();
    this->addChild(refline, 1000);

    return true;
}

void DemoScene::init_after_connect() {
    //
    this->lobby = Lobby_Layer::create();
    this->addChild(lobby, 2);

    auto menu = Menu::create();
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    this->schedule([&](float dt) { Connection::instance()->update(dt * 1000); },
                   "web_upd");
}

bool Lobby_Layer::init() {
    if (!Layer::init()) {
        return false;
    }

    auto listener = make_shared<ConnectionEventListener>(
        [&](const json& event) { notice(event); });
    Connection::instance()->addEventListener(listener, "Lobby_Layer_listener");

    this->schedule(
        [&](float) {
            if (this->is_in_game) {
                this->not_in_game->setVisible(false);
                this->in_game->setVisible(true);
            } else {
                this->not_in_game->setVisible(true);
                this->in_game->setVisible(false);
            }
        },
        "check_is_in_game");

    auto visibleSize = Director::getInstance()->getVisibleSize();

    auto sp = Sprite::create("blue_bk.png");
    sp->setPosition(visibleSize / 2);
    sp->setScaleX(visibleSize.width / 1920);
    this->addChild(sp);

    this->not_in_game = new Node();
    this->addChild(not_in_game, 1);

    this->in_game = new Node();
    this->addChild(in_game, 2);

    // not_in_game
    {
        auto menu = Menu::create();
        menu->setPosition(0, 0);
        not_in_game->addChild(menu);

        auto text =
            cocos2d::ui::TextField::create("Input Description", "font_normal.otf", 48);
        text->setMaxLengthEnabled(true);
        text->setMaxLength(10);
        text->setPosition(Vec2(800, visibleSize.height - 200));
        not_in_game->addChild(text);

        auto l = Label::createWithTTF("Create Game", "font_normal.otf", 48,
                                      Size(500, 200), TextHAlignment::CENTER,
                                      TextVAlignment::CENTER);

        auto button = MenuItemLabel::create(l, [&, text](Ref*) {
            if (text->getString() == "") {
                return;
            }
            json event;
            event["type"] = "create_game";
            event["info"] = text->getString();
            Connection::instance()->pushStatueEvent(event);
        });

        button->setPosition(800, visibleSize.height - 100);
        menu->addChild(button);

        for (int i = 0; i < 6; ++i) {
            auto info = GameInfo::create();
            not_in_game->addChild(info);
            info->setPosition(300, visibleSize.height - (i * 120 + 100));

            info->info->setString("--N/A--");

            this->game_info.push_back(info);
        }
    }

    // in_game
    {
        auto menu = Menu::create();
        menu->setPosition(0, 0);
        in_game->addChild(menu);

        this->my_uid = Label::createWithTTF("", "font_normal.otf", 48);
        my_uid->setPosition(800, visibleSize.height - 200);
        in_game->addChild(my_uid);

        auto text =
            cocos2d::ui::TextField::create("input message", "font_normal.otf", 48);
        text->setMaxLengthEnabled(true);
        text->setMaxLength(10);
        text->setPosition(Vec2(800, visibleSize.height - 400));
        in_game->addChild(text);

        // ����������Ϣ
        {
            auto l = Label::createWithTTF(
                "send message", "font_normal.otf", 48, Size(450, 100),
                TextHAlignment::CENTER, TextVAlignment::CENTER);
            auto button = MenuItemLabel::create(l, [&, text](Ref*) {
                auto str = text->getString();
                if (str == "") {
                    return;
                }

                text->setString("");

                json event;
                event["type"] = "chat_connection_say";
                event["info_1"] = str;
                event["info_2"] = "";
                Connection::instance()->pushStatueEvent(event);
            });
            button->setPosition(Vec2(800, visibleSize.height - 500));
            menu->addChild(button);
        }

        // �˳���ǰ����
        {
            auto l = Label::createWithTTF(
                "quit game", "font_normal.otf", 48, Size(450, 100),
                TextHAlignment::CENTER, TextVAlignment::CENTER);
            auto button = MenuItemLabel::create(l, [&, text](Ref*) {
                json event;
                event["type"] = "quit_game";
                event["info_1"] = Connection::instance()->getUID();
                event["info_2"] = "";
                Connection::instance()->pushStatueEvent(event);
            });
            button->setPosition(Vec2(800, visibleSize.height - 600));
            menu->addChild(button);
        }

        // ��ʼ��Ϸ
        {
            auto l = Label::createWithTTF(
                "Start Game(Only Host)", "font_normal.otf", 48, Size(650, 100),
                TextHAlignment::CENTER, TextVAlignment::CENTER);
            auto button = MenuItemLabel::create(l, [&, text](Ref*) {
                if (!this->is_host) {
                    return;
                }
                json event;
                event["type"] = "start_game";
                event["info_1"] = to_string(1004);  // seed
                Connection::instance()->pushStatueEvent(event);
            });
            button->setPosition(Vec2(800, visibleSize.height - 700));
            menu->addChild(button);
        }

        for (int i = 0; i < 6; ++i) {
            auto info = Label::createWithTTF("", "font_normal.otf", 32);
            in_game->addChild(info);
            info->setPosition(200, visibleSize.height - (i * 90 + 100));

            info->setString("N/A>>N/A");

            this->in_game_message.push_back(info);
        }
    }

    return true;
}

void Lobby_Layer::notice(const json& event) {
    if (!event.contains("type")) {
        return;
    }

    string type = event["type"];
    if (type == "game_statue") {
        vector<json> vec = event["games"];
        for (int i = 0; i < min<int>(6, vec.size()); ++i) {
            string uid = vec[i]["uid"];
            string des = vec[i]["description"];

            auto gi = game_info[i];
            gi->info->setString(uid + ">>" + des);

            gi->uid = vec[i]["uid"];
        }

        for (int i = min<int>(6, vec.size()); i < 6; ++i) {
            auto gi = game_info[i];
            gi->info->setString("--N/A--");
            gi->uid = "bad_id";
        }
        return;
    }

    if (type == "create_game_result" || type == "join_game_result") {
        string statue = event["statue"];
        if (statue == "success") {
            stringstream ss;
            ss << "UID: ";
            ss << Connection::instance()->getUID();

            this->my_uid->setString(ss.str());
            this->is_in_game = true;
            if (type == "create_game_result") {
                this->is_host = true;
            }
            return;
        }
    }

    if (type == "quit_game_result") {
        string statue = event["statue"];
        if (statue == "success") {
            this->is_in_game = false;
            return;
        }
    }

    if (type == "chat_new_message") {
        string uid = event["uid"];
        string info = event["info"];

        for (int i = 0; i < 5; ++i) {
            in_game_message[i]->setString(in_game_message[i + 1]->getString());
        }
        in_game_message[5]->setString(uid + ">>" + info);
    }

    if (type == "start_game") {
        auto s = GameScene::createScene();
        Director::getInstance()->replaceScene(s);

        string sseed = event["seed"];
        string s_player_cnt = event["player_cnt"];
        int seed = stof(sseed);
        int player_cnt = stof(s_player_cnt);
        vector<string> player_uid = event["player_uid"];
        s->start(seed, player_cnt, player_uid);
    }
}

bool Lobby_Layer::GameInfo::init() {
    if (!Node::init()) {
        return false;
    }

    this->uid = "bad_id";

    auto menu = Menu::create();
    menu->setPosition(0, 0);
    this->addChild(menu);

    this->info = Label::createWithTTF("", "font_normal.otf", 32);
    info->setPosition(0, 42);
    this->addChild(info);

    auto b =
        Label::createWithTTF("join", "font_normal.otf", 40, Size(200, 100),
                             TextHAlignment::CENTER, TextVAlignment::CENTER);
    this->button = MenuItemLabel::create(b, [&](Ref* node) {
        if (uid == "bad_id") {
            return;
        }

        json event;
        event["type"] = "join_game";
        event["info"] = uid;

        Connection::instance()->pushStatueEvent(event);
    });
    button->setPosition(0, 0);
    menu->addChild(button);

    return true;
}
