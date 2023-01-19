#include "Player1Physics.h"

#include "game/game_map/GameMap.h"

void Player1Physics::receiveEvent(GameObject* ob, const json& event) {
    string type = event["type"];
    if (type == "move") {
        float x = event["x"];

        posNow += Vec2(x, 0);

        fall_speed_y -= 6;
        fall_speed_y = max<float>(-60, fall_speed_y);
        fall_speed_y = min<float>(60, fall_speed_y);
        posNow.y += fall_speed_y;
    }
    if (type == "jump") {
        fall_speed_y = 50;
    }
    if (type == "position_force_set") {
        float x = event["x"];
        float y = event["y"];

        posNow = Vec2(x, y);
    }

    this->wall_contact_check(ob);
}

void Player1Physics::wall_contact_check(GameObject* ob) {
    const Size s = ob->getContentSize();
    // const Size s(50, 250);
    auto speed = posNow - posOld;

    auto pos = posOld;

    Vec2 pushVec(0, 0);

    iVec2 ipushVec(0, 0);

    auto maph = ob->get_game_world()->getGameMap()->getMapHelper();
    auto& map = ob->get_game_world()->getGameMap()->get();

    auto p0 = pos - Vec2(s / 2);
    auto p1 = pos + Vec2(s / 2);

    // 左下
    auto ip0 = maph->convert_in_map(p0);
    // 右上
    auto ip1 = maph->convert_in_map(p1);

    // 重新确定位置
    const auto re_check = [&]() {
        pos += pushVec;
        pushVec = Vec2(0, 0);

        p0 = pos - Vec2(s / 2);
        p1 = pos + Vec2(s / 2);
        ip0 = maph->convert_in_map(p0);
        ip1 = maph->convert_in_map(p1);
    };

    if (speed.x > 0) {
        // 右侧
        for (int i = ip0.y; i <= ip1.y; ++i) {
            auto t = map[ip1.x + 1][i];
            if (t != MapTileType::air) {
                // 会进去,要减少速度
                if (p1.x + speed.x > ip1.x * 64) {
                    speed.x = ip1.x * 64 - p1.x - 0.5;
                }
            }
        }
    } else if (speed.x < 0) {
        // 左侧
        for (int i = ip0.y; i <= ip1.y; ++i) {
            auto t = map[ip0.x - 1][i];
            if (t != MapTileType::air) {
                // 会进去,要减少速度
                if (p0.x + speed.x < (ip0.x - 1) * 64) {
                    speed.x = (ip0.x - 1) * 64 - p0.x + 0.5;
                }
            }
        }
    }

    if (speed.y > 0) {
        // 上侧
        for (int i = ip0.x; i <= ip1.x; ++i) {
            auto t = map[i][ip1.y + 1];
            if (t != MapTileType::air) {
                // 会进去,要减少速度
                if (p1.y + speed.y > ip1.y * 64) {
                    speed.y = ip1.y * 64 - p1.y - 0.5;
                }
            }
        }
    } else if (speed.y < 0) {
        // 下
        for (int i = ip0.x; i <= ip1.x; ++i) {
            auto t = map[i][ip0.y - 1];
            if (t != MapTileType::air) {
                // 会进去,要减少速度
                if (p0.y + speed.y < (ip0.y - 1) * 64) {
                    speed.y = (ip0.y - 1) * 64 - p0.y + 0.5;
                    fall_speed_y = 0;
                }
            }
        }
    }

    posNow = pos + speed;
}

void Player1Physics::updateLogic(GameObject* ob) {
    PhysicsComponent::updateLogic(ob);
}