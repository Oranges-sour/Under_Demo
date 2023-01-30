#include "GameObject.h"

GameObject* GameObject::create() {
    auto ob = new GameObject();
    if (ob) {
        ob->autorelease();
        return ob;
    }

    return nullptr;
}

bool GameObject::init(GameWorld* game_world) {
    this->_game_world = game_world;
    return true;
}

void GameObject::removeFromParent() { _game_world->removeObject(this); }

void GameObject::addGameComponent(shared_ptr<GameComponent> componet) {
    _componets.push_back(componet);
}

void GameObject::pushEvent(const json& event) {
    _componet_event_queue.push(event);
}

void GameObject::pushGameAct(const GameAct& act) {
    _componet_game_act_queue.push(act);
}

void GameObject::updateTweenAction(float rate, const std::string& key) {
    for (auto& it : _componets) {
        it->updateDraw(this, rate);
    }
}

void GameObject::mainUpdate() {
    for (auto& it : _componets) {
        it->updateLogic(this);
    }
    while (!_componet_game_act_queue.empty()) {
        auto front = _componet_game_act_queue.front();
        _componet_game_act_queue.pop();

        for (auto& it : _componets) {
            it->receiveGameAct(this, front);
        }
    }
    while (!_componet_event_queue.empty()) {
        auto front = _componet_event_queue.front();
        _componet_event_queue.pop();

        for (auto& it : _componets) {
            it->receiveEvent(this, front);
        }
    }
    for (auto& it : _componets) {
        it->updateAfterEvent(this);
    }

    this->stopAction(&_actionEase);

    _actionTween.initWithDuration(0.05f, "phy", 0.0f, 1.0f);
    _actionEase.initWithAction(&_actionTween, 1.1f);

    this->runAction(&_actionEase);

    if (this->_frame_action) {
        this->_frame_action->play(this);
    }
}

void GameObject::mainUpdateInScreenRect() {
    for (auto& it : _componets) {
        it->updateLogicInScreenRect(this);
    }
}

void GameObject::switchFrameActionStatue(
    shared_ptr<GameObjectFrameAction> new_action) {
    new_action->reset();
    this->_frame_action = new_action;
}

GameObjectFrameAction::GameObjectFrameAction(
    const vector<string>& sprite_frames,
    const function<void(GameObject*, int)>& round_call_back)
    : sprite_frames(sprite_frames),
      round_call_back(round_call_back),
      cnt(0),
      round_cnt(0) {}

void GameObjectFrameAction::reset() {
    cnt = 0;
    round_cnt = 0;
}

void GameObjectFrameAction::play(GameObject* ob) {
    ob->setSpriteFrame(sprite_frames[cnt]);
    cnt += 1;
    if (cnt == sprite_frames.size()) {
        cnt = 0;
        round_cnt += 1;
        round_call_back(ob, round_cnt);
    }
}

void WorldLight::setWorldLight(const vector<json>& lights, GameObject* ob) {
    for (auto& it : lights) {
        string key = it["key"];
        int type = it["type"];
        int r = it["r"];
        int g = it["g"];
        int b = it["b"];
        float radius = it["radius"];
        float opacity = it["opacity"];
        float offset_x = it["offset_x"];
        float offset_y = it["offset_y"];
        WorldLight light(Color3B((GLubyte)r, (GLubyte)g, (GLubyte)b), radius,
                         opacity, (WorldLight::WorldLightType)type,
                         Vec2(offset_x, offset_y));

        ob->addWorldLight(light, key);
    }
}
