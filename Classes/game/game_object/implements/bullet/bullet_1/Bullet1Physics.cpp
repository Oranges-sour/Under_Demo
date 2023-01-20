#include "Bullet1Physics.h"

#include "game/game_object/implements/particle/particle_1/Particle1.h"

void Bullet1Physics::receiveEvent(GameObject* ob, const json& event) {
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
    if (type == "contact") {
        long long data = event["object"];
        GameObject* tar = (GameObject*)data;
        auto t = tar->getGameObjectType();
        if (t == object_type_wall) {
            if (is_dead) {
                return;
            }
            is_dead = true;
            ob->setVisible(false);
            ob->removeFromParent();

            // ´´½¨Á£×Ó
            for (int i = 0; i < dead_particle_cnt; ++i) {
                auto world = ob->get_game_world();
                Particle1::create(world, dead_particle_name, ob->getPosition());
            }
        }
    }
}