#ifndef __GAME_PHYSICS_COMPONENT_H__
#define __GAME_PHYSICS_COMPONENT_H__

#include "game/game_object/GameObject.h"

class PhysicsComponent : public GameComponent {
public:
    PhysicsComponent();
    virtual void updateLogicInScreenRect(GameObject* ob) override {}
    virtual void updateDraw(GameObject* ob, float rate) override;
    virtual void receiveGameAct(GameObject* ob, const GameAct& event) override {
    }
    virtual void receiveEvent(GameObject* ob, const json& event) override {}
    virtual void updateAfterEvent(GameObject* ob) override;

    class Component;
    friend class Component;

    class WallContactComponent;
    class SpeedComponent;
    class GravityComponent;
    class ContactComponent;

    void setWallContactComponent(shared_ptr<WallContactComponent> wall_contact);
    void setSpeedComponent(shared_ptr<SpeedComponent> speed);
    void setGravityComponent(shared_ptr<GravityComponent> gravity);

private:
    void upd(GameObject* ob);

protected:
    shared_ptr<WallContactComponent> wall_contact_component;
    shared_ptr<SpeedComponent> speed_component;
    shared_ptr<GravityComponent> gravity_component;

protected:
    ActionTween action_tween;
    EaseInOut action_ease;

    Vec2 scaleNow;
    float rotationNow;
    float opacityNow;
    Vec2 posNow;

    Vec2 scaleOld;
    float rotationOld;
    float opacityOld;
    Vec2 posOld;
};

class PhysicsComponent::Component {
public:
    virtual void updateAfterEvent(GameObject* ob, PhysicsComponent* phy) = 0;
    virtual void notice(const json& event) = 0;
};

class PhysicsComponent::SpeedComponent : public PhysicsComponent::Component {
public:
    virtual void updateAfterEvent(GameObject* ob,
                                  PhysicsComponent* phy) override;
    virtual void notice(const json&) override {}

    void setSpeed(const Vec2& new_speed);
    const Vec2& getSpeed();

private:
    Vec2 speed;
};

class PhysicsComponent::WallContactComponent
    : public PhysicsComponent::Component {
public:
    WallContactComponent(
        const Vec2& left_top_offset, const Vec2& right_bottom_offset,
        shared_ptr<PhysicsComponent::SpeedComponent> speed_component);

    virtual void updateAfterEvent(GameObject* ob,
                                  PhysicsComponent* phy) override;
    virtual void notice(const json&) override {}

private:
    shared_ptr<PhysicsComponent::SpeedComponent> speed_component;
    Vec2 left_top_offset;
    Vec2 right_bottom_offset;
};

class PhysicsComponent::GravityComponent : public PhysicsComponent::Component {
public:
    GravityComponent(
        float g, shared_ptr<PhysicsComponent::SpeedComponent> speed_component);

    virtual void updateAfterEvent(GameObject* ob,
                                  PhysicsComponent* phy) override;
    virtual void notice(const json&) override {}

private:
    shared_ptr<PhysicsComponent::SpeedComponent> speed_component;
    float g;
};

#endif
