#include "StartPointAI.h"

#include "game/game_object/implements/particle/particle_1/Particle1.h"
#include "utility/math/MyMath.h"

StartPointAI::StartPointAI(float particle_cnt_per_frame,
                           const string& particle_name, const Vec2& offset_1,
                           const Vec2& offset_2)
    : particle_cnt_per_frame(particle_cnt_per_frame),
      particle_name(particle_name),
      offset_1(offset_1),
      offset_2(offset_2),
      sum(0.0f) {}

void StartPointAI::updateLogicInScreenRect(GameObject* ob) {
    sum += particle_cnt_per_frame;

    int cc = 0;

    while (sum > 1.0f) {
        sum -= 1.0f;
        cc += 1;
    }

    auto world = ob->getGameWorld();

    auto pos = ob->getPosition();

    rand_bool r;

    auto ran = world->getGlobalRandom();

    for (int i = 0; i < cc; ++i) {
        if (r(*ran)) {
            Particle1::create(world, particle_name, pos + offset_1);
        } else {
            Particle1::create(world, particle_name, pos + offset_2);
        }
    }
}
