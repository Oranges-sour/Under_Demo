#include "Bullet2Physics.h"

#include "game/game_object/implements/particle/particle_1/Particle1.h"
#include "utility/math/MyMath.h"

Bullet2Physics::Bullet2Physics(const Vec2& start_pos, const Vec2& end_pos,
                               float move_speed, int particle_explode_cnt,
                               const string& particle_explode_json_key)
    : _move_speed(move_speed),
      _particle_explode_cnt(particle_explode_cnt),
      _particle_explode_json_key(particle_explode_json_key),
      _is_dead(false) {
    this->posNow = start_pos;
    this->rotationNow = MyMath::getRotation(start_pos, end_pos) - 90;
}

void Bullet2Physics::receiveEvent(GameObject* ob, const GameEvent& event) {
    if (event.type == GameEventType::move) {
        Vec2 m;
        m.x = event.param1.f_val;
        m.y = event.param2.f_val;

        posNow += m * _move_speed;
    }

     if (event.type == GameEventType::contact) {
         GameObject* tar = event.param1.p_val;
         auto t = tar->getGameObjectType();
         if (t == object_type_wall || t == object_type_player) {
             if (_is_dead) {
                 return;
             }
             _is_dead = true;
             ob->setVisible(false);
             ob->removeFromParent();

            // ´´½¨Á£×Ó
            for (int i = 0; i < _particle_explode_cnt; ++i) {
                auto world = ob->getGameWorld();
                Particle1::create(world, _particle_explode_json_key,
                                  ob->getPosition());
            }
        }
    }
}
