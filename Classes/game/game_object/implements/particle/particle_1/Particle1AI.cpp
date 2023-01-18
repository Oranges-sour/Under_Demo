#include "Particle1AI.h"

void Particle1AI::updateLogic(GameObject* ob) {
    {
        json event;
        event["type"] = "move";
        event["x"] = xx * 60;
        event["y"] = yy * 60;

        ob->pushEvent(event);
    }
    {
        json event;
        event["type"] = "rotate";
        event["r"] = 40;

        ob->pushEvent(event);
    }

    auto light = ob->getWorldLight("main_light");
    light->radius = light->radius / 2.0f;

    cnt += 1;
    if (cnt >= 5) {
        ob->removeFromParent();
    }
}