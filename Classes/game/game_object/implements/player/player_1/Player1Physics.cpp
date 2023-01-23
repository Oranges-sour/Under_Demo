#include "Player1Physics.h"

#include "game/game_map/GameMap.h"

Player1Physics::Player1Physics(const Vec2& pos) : run(false) {
    this->posNow = pos;

    frame_stay = make_shared<GameObjectFrameAction>(
        vector<string>{"man_stay.png"}, [&](GameObject*, int) {});

    frame_run = make_shared<GameObjectFrameAction>(
        vector<string>{"man_run_1.png", "man_run_2.png", "man_run_3.png",
                       "man_run_4.png"},
        [&](GameObject*, int) {});

    frame_jump = make_shared<GameObjectFrameAction>(
        vector<string>{"man_jump_1.png", "man_jump_2.png", "man_jump_3.png"},
        [&](GameObject* ob, int) {
            ob->switch_frame_action_statue(frame_jump_stay);
        });

    frame_jump_stay = make_shared<GameObjectFrameAction>(
        vector<string>{"man_jump_3.png"}, [&](GameObject* ob, int c) {
            if (c >= 8) {
                if (run) {
                    ob->switch_frame_action_statue(frame_run);
                } else {
                    ob->switch_frame_action_statue(frame_stay);
                }
            }
        });
}

void Player1Physics::receiveEvent(GameObject* ob, const json& event) {
    string type = event["type"];
    if (type == "move") {
        if (speed_component) {
            float x = event["x"];

            auto speed = speed_component->getSpeed();
            speed.x += x;
            speed_component->setSpeed(speed);

            if (abs(speed.x) > 1) {
                run = true;
                ob->switch_frame_action_statue(frame_run);
            } else {
                ob->switch_frame_action_statue(frame_stay);
                run = false;
            }
            if (speed.x < 0) {
                scaleNow.x = -1;
            } else if (speed.x > 0) {
                scaleNow.x = 1;
            }
        }
    }
    if (type == "jump") {
        if (speed_component) {
            ob->switch_frame_action_statue(frame_jump);

            auto speed = speed_component->getSpeed();
            speed.y = 50;
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
}