#include "Bullet1AI.h"

void Bullet1AI::updateLogic(GameObject* ob) {
    {
        json event;
        event["type"] = "move";
        event["x"] = direction.x * move_speed;
        event["y"] = direction.y * move_speed;

        ob->pushEvent(event);
    }
    {
        json event;
        event["type"] = "rotate";
        event["r"] = rotate_speed;

        ob->pushEvent(event);
    }
}