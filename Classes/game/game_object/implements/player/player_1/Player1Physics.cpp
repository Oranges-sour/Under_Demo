#include "Player1Physics.h"

#include "game/game_map/GameMap.h"

Player1Physics::Player1Physics(const Vec2& start_pos, float move_speed,
                               float jump_speed,
                               const vector<string>& frame_action_stay,
                               const vector<string>& frame_action_run,
                               const vector<string>& frame_action_jump,
                               const vector<string>& frame_action_jump_stay)
    : run(false), move_speed(move_speed), jump_speed(jump_speed) {
    this->posNow = start_pos;

    frame_stay = make_shared<GameObjectFrameAction>(frame_action_stay,
                                                    [&](GameObject*, int) {});
    frame_run = make_shared<GameObjectFrameAction>(frame_action_run,
                                                   [&](GameObject*, int) {});
    frame_jump = make_shared<GameObjectFrameAction>(
        frame_action_jump, [&](GameObject* ob, int) {
            ob->switch_frame_action_statue(frame_jump_stay);
        });
    frame_jump_stay = make_shared<GameObjectFrameAction>(
        frame_action_jump_stay, [&](GameObject* ob, int c) {
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
            float x = event["x"] * move_speed;

            auto speed = speed_component->getSpeed();
            speed.x += x;
            speed_component->setSpeed(speed);

            if (abs(speed.x) > 0.1) {
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
}