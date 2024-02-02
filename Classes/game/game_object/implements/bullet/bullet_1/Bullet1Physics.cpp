#include "Bullet1Physics.h"

#include "game/game_object/implements/particle/particle_1/Particle1.h"

Bullet1Physics::Bullet1Physics(const Vec2& pos, const Vec2& direction,
                               float move_speed, float rotate_speed,
                               int dead_particle_cnt,
                               const string& dead_particle_name)
    : is_dead(false),
      dead_particle_cnt(dead_particle_cnt),
      dead_particle_name(dead_particle_name),
      direction(direction),
      move_speed(move_speed),
      rotate_speed(rotate_speed) {

    this->posNow = pos;
    this->schedule([&](GameObject* ob) { upd(ob); }, 0, "upd");
}

void Bullet1Physics::receiveEvent(GameObject* ob, const GameEvent& event) {
    //TODO :
    //string type = event["type"];
    //if (type == "contact") {
    //    long long data = event["object"];
    //    GameObject* tar = (GameObject*)data;
    //    auto t = tar->getGameObjectType();
    //    if (t == object_type_wall) {
    //        if (is_dead) {
    //            return;
    //        }
    //        is_dead = true;
    //        ob->setVisible(false);
    //        ob->removeFromParent();

    //        // ´´½¨Á£×Ó
    //        for (int i = 0; i < dead_particle_cnt; ++i) {
    //            auto world = ob->getGameWorld();
    //            Particle1::create(world, dead_particle_name, ob->getPosition());
    //        }
    //    }
    //}
}

void Bullet1Physics::upd(GameObject* ob) {
    posNow += direction * move_speed;
    rotationNow += rotate_speed;
}
