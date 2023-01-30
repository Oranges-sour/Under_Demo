#include "Player1Physics.h"

#include "game/game_map/GameMap.h"

Player1Physics::Player1Physics(const Vec2& start_pos, float move_speed,
                               float jump_speed,
                               const vector<string>& frame_action_stay,
                               const vector<string>& frame_action_run,
                               const vector<string>& frame_action_jump,
                               const vector<string>& frame_action_jump_stay,
                               const vector<string>& frame_action_attack_near)
    : run(false),
      move_speed(move_speed),
      jump_speed(jump_speed),
      attack(0),
      on_attack(false) {
    this->posNow = start_pos;

    frame_stay = make_shared<GameObjectFrameAction>(frame_action_stay,
                                                    [&](GameObject*, int) {});
    frame_run = make_shared<GameObjectFrameAction>(frame_action_run,
                                                   [&](GameObject*, int) {});
    frame_jump = make_shared<GameObjectFrameAction>(
        frame_action_jump, [&](GameObject* ob, int) {
            ob->switchFrameActionStatue(frame_jump_stay);
        });
    frame_jump_stay = make_shared<GameObjectFrameAction>(
        frame_action_jump_stay, [&](GameObject* ob, int c) {
            if (c >= 8) {
                if (run) {
                    ob->switchFrameActionStatue(frame_run);
                } else {
                    ob->switchFrameActionStatue(frame_stay);
                }
            }
        });

    frame_attack_near = make_shared<GameObjectFrameAction>(
        frame_action_attack_near, [&](GameObject* ob, int) {});
}

void Player1Physics::receiveEvent(GameObject* ob, const json& event) {
    string type = event["type"];
    if (type == "move") {
        if (speed_component) {
            float x = event["x"] * move_speed;

            auto speed = speed_component->getSpeed();
            speed.x += x;
            speed_component->setSpeed(speed);

            if (abs(speed.x) > 0.1) {
                run = true;
            } else {
                run = false;
            }

            if (on_attack) {
                return;
            }

            if (run) {
                ob->switchFrameActionStatue(frame_run);
            } else {
                ob->switchFrameActionStatue(frame_stay);
            }
            if (speed.x < 0) {
                scaleNow.x = -1;
            } else if (speed.x > 0) {
                scaleNow.x = 1;
            }
        }
    }
    if (type == "attack") {
        attack += event["x"];
        if (abs(attack) > 0.1) {
            on_attack = true;
            ob->switchFrameActionStatue(frame_attack_near);
        } else {
            if (run) {
                ob->switchFrameActionStatue(frame_run);
            } else {
                ob->switchFrameActionStatue(frame_stay);
            }
            on_attack = false;
        }
        if (attack < 0) {
            scaleNow.x = -1;
        } else if (attack > 0) {
            scaleNow.x = 1;
        }
    }
    if (type == "jump") {
        if (speed_component) {
            ob->switchFrameActionStatue(frame_jump);

            auto speed = speed_component->getSpeed();
            speed.y = jump_speed;
            speed_component->setSpeed(speed);
        }
    }
    if (type == "position_force_set") {
        float x = event["x"];
        float y = event["y"];

        posNow = Vec2(x, y);
    }
}

void Player1Physics::updateLogic(GameObject* ob) {
    PhysicsComponent::updateLogic(ob);

    if (on_attack) {
        if (speed_component) {
            auto sp1 = dynamic_pointer_cast<Player1Speed>(speed_component);
            sp1->add_speed_rate(0.1, "attack_rate");
        }
    } else {
        if (speed_component) {
            auto sp1 = dynamic_pointer_cast<Player1Speed>(speed_component);
            sp1->remove_speed_rate("attack_rate");
        }
    }
}

/////////////////////////////////////////////////////

void Player1Speed::updateAfterEvent(GameObject* ob, PhysicsComponent* phy) {
    float r = 1;
    for (auto& it : speed_rate) {
        r *= it.second;
    }

    this->setSpeed(this->getSpeed() * r);
    PhysicsComponent::SpeedComponent::updateAfterEvent(ob, phy);
    this->setSpeed(this->getSpeed() / r);
}

void Player1Speed::add_speed_rate(float rate, const string& key) {
    speed_rate.insert({key, rate});
}

void Player1Speed::remove_speed_rate(const string& key) {
    speed_rate.erase(key);
}
