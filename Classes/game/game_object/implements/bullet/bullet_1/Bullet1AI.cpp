#include "Bullet1AI.h"

void Bullet1AI::updateLogic(GameObject* ob) {
    {
        json event;
        event["type"] = "move";
        event["x"] = xx * 40;
        event["y"] = yy * 40;

        ob->pushEvent(event);
    }
    {
        json event;
        event["type"] = "rotate";
        event["r"] = 40;

        ob->pushEvent(event);
    }
}