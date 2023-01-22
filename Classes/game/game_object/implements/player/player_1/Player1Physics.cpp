#include "Player1Physics.h"

#include "game/game_map/GameMap.h"

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
            } else {
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
    const vector<string> vec{"man_run_1.png", "man_run_2.png", "man_run_3.png",
                             "man_run_4.png"};

    cnt += 1;

    if (run) {
        ob->setSpriteFrame(vec[cnt % 4]);
    } else {
        ob->setSpriteFrame("man_stay.png");
    }

    PhysicsComponent::updateLogic(ob);
}