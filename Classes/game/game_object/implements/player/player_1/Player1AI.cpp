#include "Player1AI.h"

#include "game/game_object/implements/bullet/bullet_1/Bullet1.h"

void Player1AI::updateLogic(GameObject* ob) {
    json event;
    event["type"] = "move";
    event["x"] = xx;

    ob->pushEvent(event);
}

void Player1AI::receiveEvent(GameObject* ob, const json& event) {}

void Player1AI::receiveGameAct(GameObject* ob, const GameAct& event) {
    const float SPEED = 25;

    if (event.type == act_move_start) {
        if (ob->getUID() == event.uid) {
            xx += event.param1 * SPEED;
        }
    }
    if (event.type == act_move_stop) {
        if (ob->getUID() == event.uid) {
            xx -= event.param1 * SPEED;
        }
    }
    if (event.type == act_jump) {
        json event;
        event["type"] = "jump";

        ob->pushEvent(event);
    }
    if (event.type == act_attack) {
        auto world = ob->get_game_world();
        Bullet1::create(world, ob->getPosition(),
                        Vec2(event.param1, event.param2));
    }
    if (event.type == act_position_force_set) {
        json ee;
        ee["type"] = "position_force_set";
        ee["x"] = event.param1;
        ee["y"] = event.param2;

        ob->pushEvent(ee);
    }
}