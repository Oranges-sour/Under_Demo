#include "Particle1AI.h"

void Particle1AI::updateLogic(GameObject* ob) {
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

    auto& lights = ob->getAllWorldLight();
    for (auto& it : lights) {
        it.second.radius *= light_decrease_rate;
    }

    cnt += 1;
    if (cnt > live_frame) {
        ob->removeFromParent();
    }
}