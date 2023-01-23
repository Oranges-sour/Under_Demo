#include "PhysicsComponent.h"

#include "game/game_map/GameMap.h"

void PhysicsComponent::updateLogic(GameObject* ob) {
    posOld = posNow;
    rotationOld = rotationNow;
    opacityOld = opacityNow;
    scaleOld = scaleNow;
}

void PhysicsComponent::updateDraw(GameObject* ob, float rate) {
    auto deltaPos = posNow - posOld;
    ob->setPosition(posOld + deltaPos * rate);

    auto deltaRotation = rotationNow - rotationOld;
    ob->setRotation(rotationOld + deltaRotation * rate);

    auto deltaOpacity = opacityNow - opacityOld;
    ob->setOpacity((opacityOld + deltaOpacity * rate) * 255);

    auto deltaScale = scaleNow - scaleOld;
    ob->setScaleX(scaleOld.x + deltaScale.x * rate);
    ob->setScaleY(scaleOld.y + deltaScale.y * rate);
}

void PhysicsComponent::updateAfterEvent(GameObject* ob) {
    if (gravity_component) {
        gravity_component->updateAfterEvent(ob, this);
    }
    if (speed_component) {
        speed_component->updateAfterEvent(ob, this);
    }
    if (wall_contact_component) {
        wall_contact_component->updateAfterEvent(ob, this);
    }
}

void PhysicsComponent::setWallContactComponent(
    shared_ptr<WallContactComponent> wall_contact) {
    this->wall_contact_component = wall_contact;
}

void PhysicsComponent::setSpeedComponent(shared_ptr<SpeedComponent> speed) {
    this->speed_component = speed;
}

void PhysicsComponent::setGravityComponent(
    shared_ptr<GravityComponent> gravity) {
    this->gravity_component = gravity;
}

PhysicsComponent::WallContactComponent::WallContactComponent(
    const Vec2& left_top_offset, const Vec2& right_bottom_offset,
    shared_ptr<PhysicsComponent::SpeedComponent> speed_component)
    : left_top_offset(left_top_offset),
      right_bottom_offset(right_bottom_offset),
      speed_component(speed_component) {}

void PhysicsComponent::WallContactComponent::updateAfterEvent(
    GameObject* ob, PhysicsComponent* phy) {
    auto speed = phy->posNow - phy->posOld;

    auto pos = phy->posOld;

    auto maph = ob->get_game_world()->getGameMap()->getMapHelper();
    auto& map = ob->get_game_world()->getGameMap()->get();

    auto p0 = pos + left_top_offset;
    auto p1 = pos + right_bottom_offset;

    // 左下
    auto ip0 = maph->convert_in_map(p0);
    // 右上
    auto ip1 = maph->convert_in_map(p1);

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

                    if (speed_component) {
                        auto cspeed = speed_component->getSpeed();
                        cspeed.y = 0;
                        speed_component->setSpeed(cspeed);
                    }
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
                    if (speed_component) {
                        auto cspeed = speed_component->getSpeed();
                        cspeed.y = 0;
                        speed_component->setSpeed(cspeed);
                    }
                }
            }
        }
    }

    phy->posNow = pos + speed;
}

PhysicsComponent::GravityComponent::GravityComponent(
    float g, shared_ptr<PhysicsComponent::SpeedComponent> speed_component)
    : g(g), speed_component(speed_component) {}

void PhysicsComponent::GravityComponent::updateAfterEvent(
    GameObject* ob, PhysicsComponent* phy) {
    if (speed_component) {
        auto speed = speed_component->getSpeed();
        speed.y -= g;
        speed_component->setSpeed(speed);
    }
}

void PhysicsComponent::SpeedComponent::updateAfterEvent(GameObject* ob,
                                                        PhysicsComponent* phy) {
    speed.x = max(min(speed.x, 60.0f), -60.0f);
    speed.y = max(min(speed.y, 60.0f), -60.0f);

    phy->posNow += speed;
}

void PhysicsComponent::SpeedComponent::setSpeed(const Vec2& new_speed) {
    this->speed = new_speed;
}

const Vec2& PhysicsComponent::SpeedComponent::getSpeed() { return this->speed; }
