#include "Particle1AI.h"

Particle1AI::Particle1AI(const Vec2& direction, int live_frame,
                         float move_speed, float rotate_speed,
                         float light_decrease_rate)
    : live_frame(live_frame),
      direction(direction),
      move_speed(move_speed),
      rotate_speed(move_speed),
      light_decrease_rate(light_decrease_rate) {
    this->schedule([&](GameObject* ob) { upd(ob); }, 0, "ai");
}

void Particle1AI::upd(GameObject* ob) {
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
