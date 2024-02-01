#include "GameFrame.h"

#include "game/game_world/GameWorld.h"

GameFrameManager::GameFrameManager() { _next_frame = make_shared<GameFrame>(); }

void GameFrameManager::init(GameWorld* game_world) {
    this->_game_world = game_world;

    auto listener = make_shared<ConnectionEventListener>(
        [&](const json& event) { this->noitce(event); });

    Connection::instance()->addEventListener(listener,
                                                   "frame_manager_listener");
}

void GameFrameManager::release() {
    Connection::instance()->removeEventListener("frame_manager_listener");
}

void GameFrameManager::noitce(const json& frame_event) {
    string type = frame_event["type"];
    if (type != "frame") {
        return;
    }
    auto frame = this->generateFrame(frame_event);

    for (auto& it : frame->_actions) {
        _game_world->pushGameAct(it);
    }
}

void GameFrameManager::update() {
    if (_next_frame->_actions.size() == 0) {
        return;
    }

    /*auto js = this->generateJson(_next_frame);
    Connection::instance()->pushStatueEvent(js);*/

    _next_frame = make_shared<GameFrame>();
}

shared_ptr<GameFrame> GameFrameManager::generateFrame(const json& event) {
    auto game_frame = make_shared<GameFrame>();

    string type = event["type"];
    if (type != "frame") {
        return game_frame;
    }

    json frame = event["frame"];

    vector<json> vec = frame["actions"];
    for (auto& it : vec) {
        GameAct act;

        act.uid = it["uid"];
        act.type = it["type"];
        act.param1 = it["param1"];
        act.param2 = it["param2"];
        act.param3 = it["param3"];
        act.param4 = it["param4"];

        game_frame->_actions.push_back(act);
    }

    return game_frame;
}

json GameFrameManager::generateJson(shared_ptr<GameFrame> game_frame) {
    json js;
    js["type"] = "frame";

    json frame;
    frame["player"] = Connection::instance()->getUID();

    vector<json> vec;
    for (auto& it : _next_frame->_actions) {
        json act;
        act["uid"] = it.uid;
        act["type"] = it.type;
        act["param1"] = it.param1;
        act["param2"] = it.param2;
        act["param3"] = it.param3;
        act["param4"] = it.param4;

        vec.push_back(act);
    }
    frame["actions"] = vec;

    js["frame"] = frame;

    return js;
}

void GameFrameManager::pushGameAct(const GameAct& act, bool role_back) {
    _next_frame->_actions.push_back(act);
    if (role_back) {
        // Á¢¼´·¢»Ø
        _game_world->pushGameAct(act);
    }
}
