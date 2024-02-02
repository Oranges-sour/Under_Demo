#include "Particle1Physics.h"

Particle1Physics::Particle1Physics(const Vec2& pos, float scale_decrease_rate,
                                   float opacity_decrease_rate)
    : scale_decrease_rate(scale_decrease_rate),
      opacity_decrease_rate(opacity_decrease_rate) {
    this->schedule([&](GameObject* ob) { upd(ob); }, 0, "phy");
    this->posNow = pos;
}

void Particle1Physics::receiveEvent(GameObject* ob, const GameEvent& event) {
    if (event.type == GameEventType::move) {
        float x = event.param1.f_val;
        float y = event.param2.f_val;
        posNow += Vec2(x, y);
    }

    if (event.type == GameEventType::rotate) {
        float r = event.param1.f_val;
        rotationNow += r;
    }
}

void Particle1Physics::upd(GameObject* ob) {
    scaleNow = scaleNow * scale_decrease_rate;
    opacityNow = opacityNow * opacity_decrease_rate;
}
