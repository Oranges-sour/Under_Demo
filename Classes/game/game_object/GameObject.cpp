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
    this->game_world = game_world;
    return true;
}

void GameObject::removeFromParent() { game_world->removeObject(this); }

void GameObject::addGameComponent(shared_ptr<GameComponent> componet) {
    _componets.push_back(componet);
}

void GameObject::pushEvent(const json& event) {
    _componetEventQueue.push(event);
}

void GameObject::pushGameAct(const GameAct& act) {
    _componetGameActQueue.push(act);
}

void GameObject::updateTweenAction(float rate, const std::string& key) {
    for (auto& it : _componets) {
        it->updateDraw(this, rate);
    }
}

void GameObject::main_update() {
    for (auto& it : _componets) {
        it->updateLogic(this);
    }
    while (!_componetGameActQueue.empty()) {
        auto front = _componetGameActQueue.front();
        _componetGameActQueue.pop();

        for (auto& it : _componets) {
            it->receiveGameAct(this, front);
        }
    }
    while (!_componetEventQueue.empty()) {
        auto front = _componetEventQueue.front();
        _componetEventQueue.pop();

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

void GameObject::main_update_in_screen_rect() {
    for (auto& it : _componets) {
        it->updateLogicInScreenRect(this);
    }
}

void GameObject::switch_frame_action_statue(
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
