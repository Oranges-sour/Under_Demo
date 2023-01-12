#include "GameFrame.h"

GameFrameManager::GameFrameManager() {
    for (int i = 0; i < 36000; ++i) {
        frames.push_back(GameFrame());
    }

    // ╤сапн╙©у
    head = 0;
    tail = -1;

    _next_frame = make_shared<GameFrame>();
}

void GameFrameManager::receiveFrameFromServer(const json& frame_event) {
    auto game_frame = make_shared<GameFrame>();

    string type = frame_event["type"];
    if (type != "frame") {
        return;
    }

    json frame = frame_event["frame"];

    vector<json> vec = frame["actions"];
    for (auto& it : vec) {
        GameAct act;

        act.uid = it["uid"];
        act.type = it["type"];
        act.param1 = it["param1"];
        act.param2 = it["param2"];
        act.param3 = it["param3"];
        act.param4 = it["param4"];

        game_frame->actions.push_back(act);
    }

    tail += 1;
    this->frames[tail] = *game_frame;
}

json GameFrameManager::generateJsonOfNextFrame(const string& connection_uid) {
    json js;
    js["type"] = "frame";

    json frame;
    frame["cnt"] = tail + 1;
    frame["player"] = connection_uid;

    vector<json> vec;
    for (auto& it : _next_frame->actions) {
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

void GameFrameManager::newNextFrame() {
    this->_next_frame = make_shared<GameFrame>();
}

bool GameFrameManager::hasNewFrame() { return (head <= tail); }

GameFrame* GameFrameManager::getNextFrame() {
    if (!hasNewFrame()) {
        return nullptr;
    }

    auto p = &frames[head];
    head += 1;

    return p;
}

void GameFrameManager::pushGameAct(const GameAct& act) {
    _next_frame->actions.push_back(act);
}
