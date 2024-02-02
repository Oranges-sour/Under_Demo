#include "Bullet2AI.h"

#include "game/game_object/implements/particle/particle_1/Particle1.h"
#include "utility/json/json.h"

Bullet2AI::Bullet2AI(const Vec2& start_pos, const Vec2& end_pos,
                     const string& particle_move_json_key,
                     float particle_move_cnt_per_frame)
    : _particle_move_json_key(particle_move_json_key),
      _particle_move_cnt_per_frame(particle_move_cnt_per_frame),
      _particle_move_cnt(0.0) {
    _direction = (end_pos - start_pos).getNormalized();

    this->schedule([&](GameObject* ob) { create_particle(ob); }, 0,
                   "create_particle");
    this->schedule([&](GameObject* ob) { upd(ob); }, 0, "update");
}

void Bullet2AI::upd(GameObject* ob) {
    GameEvent event{GameEventType::move, "", 0, 0, 0, ""};
    event.param1.f_val = _direction.x;
    event.param2.f_val = _direction.y;
    ob->pushEvent(event);
}

void Bullet2AI::create_particle(GameObject* ob) {
    _particle_move_cnt += _particle_move_cnt_per_frame;
    while (_particle_move_cnt > 1.0) {
        _particle_move_cnt -= 1.0;
        Particle1::create(ob->getGameWorld(), _particle_move_json_key,
                          ob->getPosition());
    }
}
