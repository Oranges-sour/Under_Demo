#include "Particle1Physics.h"

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