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
}

void GameObject::main_update_in_screen_rect() {
    for (auto& it : _componets) {
        it->updateLogicInScreenRect(this);
    }
}