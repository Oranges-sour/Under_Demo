#include "Particle1Physics.h"

Particle1Physics::Particle1Physics(const Vec2& pos, float scale_decrease_rate,
                                   float opacity_decrease_rate)
    : scale_decrease_rate(scale_decrease_rate),
      opacity_decrease_rate(opacity_decrease_rate) {
    this->posNow = pos;
}

void Particle1Physics::receiveEvent(GameObject* ob, const json& event) {
    string type = event["type"];
    if (type == "move") {
        float x = event["x"];
        float y = event["y"];
        posNow += Vec2(x, y);
        return;
    }
    if (type == "rotate") {
        float r = event["r"];
        rotationNow += r;
        return;
    }
}

void Particle1Physics::updateLogic(GameObject* ob) {
    PhysicsComponent::updateLogic(ob);
    scaleNow = scaleNow * scale_decrease_rate;
    opacityNow = opacityNow * opacity_decrease_rate;
}
