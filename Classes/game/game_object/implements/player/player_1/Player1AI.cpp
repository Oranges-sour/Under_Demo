#include "Player1AI.h"

#include "game/game_object/implements/bullet/bullet_1/Bullet1.h"

void Player1AI::updateLogic(GameObject* ob) {}

void Player1AI::receiveEvent(GameObject* ob, const json& event) {}

void Player1AI::receiveGameAct(GameObject* ob, const GameAct& event) {
    if (event.type == act_move_start) {
        if (ob->getUID() == event.uid) {
            json ee;
            ee["type"] = "move";
            ee["x"] = event.param1;
            ob->pushEvent(ee);
        }
    }
    if (event.type == act_move_stop) {
        if (ob->getUID() == event.uid) {
            json ee;
            ee["type"] = "move";
            ee["x"] = -event.param1;
            ob->pushEvent(ee);
        }
    }
    if (event.type == act_jump) {
        json event;
        event["type"] = "jump";
        ob->pushEvent(event);
    }
    if (event.type == act_attack_start) {
        json ee;
        ee["type"] = "attack";
        ee["x"] = event.param1;
        ob->pushEvent(ee);
        // auto world = ob->get_game_world();
        // Bullet1::create(world, "bullet_1", ob->getPosition(),
        //                 Vec2(event.param1, event.param2));
    }
    if (event.type == act_attack_stop) {
        json ee;
        ee["type"] = "attack";
        ee["x"] = -event.param1;
        ob->pushEvent(ee);
    }
    if (event.type == act_position_force_set) {
        json ee;
        ee["type"] = "position_force_set";
        ee["x"] = event.param1;
        ee["y"] = event.param2;

        ob->pushEvent(ee);
    }
}