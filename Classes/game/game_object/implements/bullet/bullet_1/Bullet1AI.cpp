#include "Bullet1AI.h"

Bullet1AI::Bullet1AI(const Vec2& direction, float move_speed,
                     float rotate_speed)
    : direction(direction), move_speed(move_speed), rotate_speed(rotate_speed) {
    this->schedule([&](GameObject* ob) { upd(ob); }, 0, "bullet_ai");
}

void Bullet1AI::upd(GameObject* ob) {
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