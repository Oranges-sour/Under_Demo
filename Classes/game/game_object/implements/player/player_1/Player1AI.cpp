#include "Player1AI.h"

#include "game/game_object/implements/bullet/bullet_1/Bullet1.h"

Player1AI::Player1AI() {}

void Player1AI::receiveEvent(GameObject* ob, const GameEvent& event) {
    if (event.type == GameEventType::control_move_start) {
        GameEvent _event{GameEventType::move, "", 0, 0, 0, ""};

        _event.param1.f_val = event.param1.f_val;
        ob->pushEvent(_event);
    }

    if (event.type == GameEventType::control_move_stop) {

        GameEvent _event{GameEventType::move, "", 0, 0, 0, ""};

        _event.param1.f_val = -event.param1.f_val;
        ob->pushEvent(_event);
    }

    if (event.type == GameEventType::control_jump) {
        GameEvent _event{GameEventType::jump, "", 0, 0, 0, ""};

        ob->pushEvent(_event);
    }
    if (event.type == GameEventType::control_attack_start) {
        GameEvent _event{GameEventType::other_compoment, "attack", 0, 0, 0, ""};

        _event.param1.f_val = event.param1.f_val;
        ob->pushEvent(_event);
        // auto world = ob->getGameWorld();
        // Bullet1::create(world, "bullet_1", ob->getPosition(),
        //                 Vec2(event.param1, event.param2));
    }
    if (event.type == GameEventType::control_attack_stop) {
        GameEvent _event{GameEventType::other_compoment, "attack", 0, 0, 0, ""};

        _event.param1.f_val = -event.param1.f_val;
        ob->pushEvent(_event);
    }

}