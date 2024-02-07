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
    _components.push_back(componet);
}

void GameObject::pushEvent(const GameEvent& event) {
    _component_event_queue.push(event);
}

void GameObject::mainUpdate() {
    
    for (auto& it : _components) {
        it->updateLogic(this);
    }

    while (!_component_event_queue.empty()) {
        const auto& front = _component_event_queue.front();
        

        for (auto& it : _components) {
            it->receiveEvent(this, front);
        }

        _component_event_queue.pop();
    }
    for (auto& it : _components) {
        it->updateAfterEvent(this);
    }

    for (auto& it : _components) {
        it->updateDraw(this, 1.0f);
    }

    if (this->_frame_action) {
        this->_frame_action->play(this);
    }
}

void GameObject::mainUpdateInScreenRect() {
    for (auto& it : _components) {
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

void GameComponent::schedule(const function<void(GameObject* ob)>& call_back,
                             int interval, const string& key, int repeat_time,
                             int first_run_delay, int order) {
    _schedule_need_to_add.push_back(ScheduleBag(
        call_back, interval, repeat_time, first_run_delay, order, key));
}

void GameComponent::scheduleOnce(
    const function<void(GameObject* ob)>& call_back, int delay,
    const string& key) {
    this->schedule(call_back, 0, key, 1, delay);
}

void GameComponent::unschedule(const string& key) {
    _schedule_need_to_erase.push_back(key);
}

void GameComponent::updateLogic(GameObject* ob) {
    for (auto& it : _schedule_need_to_add) {
        for (auto& m : _schedule_bag) {
            auto iter = m.second.find(it._key);
            if (iter != m.second.end()) {
                CCLOG(
                    "GameComponent::schedule(...): this key has existed "
                    "before.");
                m.second.erase(iter);
            }
        }

        if (_schedule_bag.find(it._order) == _schedule_bag.end()) {
            _schedule_bag.insert({it._order, map<string, ScheduleBag>()});
        }

        _schedule_bag.find(it._order)->second.insert({it._key, it});
    }
    _schedule_need_to_add.clear();

    for (auto& m : _schedule_bag) {
        for (auto& it : m.second) {
            auto& bag = it.second;
            if (bag._first_run_delay > 0) {
                bag._first_run_delay -= 1;
                continue;
            }

            if (bag._repeat_time == 0) {
                this->unschedule(it.first);
                continue;
            }

            bag._cnt += 1;
            if (bag._cnt > bag._interval) {
                bag._cnt = 0;
                bag._call_back(ob);
                bag._repeat_time -= 1;
            }
        }
    }

    for (auto& it : _schedule_need_to_erase) {
        for (auto& m : _schedule_bag) {
            m.second.erase(it);
        }
    }
    _schedule_need_to_erase.clear();
}
