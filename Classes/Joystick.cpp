#include "Joystick.h"

#include "MyMath.h"
#include "TouchesPool.h"

Joystick* Joystick::create(const std::string& backGround,
                           const std::string& stick, const std::string& effect)
{
    auto joystick = new (std::nothrow) Joystick();
    if (joystick && joystick->init(backGround, stick, effect)) {
        joystick->autorelease();
        return joystick;
    }
    CC_SAFE_DELETE(joystick);
    return nullptr;
}

bool Joystick::init(const std::string& backGround, const std::string& stick,
                    const std::string& effect)
{
    this->backGround = Sprite::createWithSpriteFrameName(backGround);
    this->backGround->setLocalZOrder(1);
    this->addChild(this->backGround);
    touchRadius = this->backGround->getContentSize().width / 2;

    this->effect = Sprite::createWithSpriteFrameName(effect);
    this->effect->setLocalZOrder(2);
    this->effect->setOpacity(0);
    this->addChild(this->effect);

    this->stick = Sprite::createWithSpriteFrameName(stick);
    this->stick->setLocalZOrder(3);
    this->addChild(this->stick);

    this->schedule([&](float) { _update(); }, "_update");

    return true;
}

void Joystick::setOriginalPosition(const Vec2& position)
{
    backGround->setPosition(position);
    effect->setPosition(position);
    stick->setPosition(position);
    originalPos = position;
}

Vec2 Joystick::getMoveVec()
{
    auto stickPos = stick->getPosition();
    if (stickPos == originalPos) {
        return Vec2::ZERO;
    }
    return MyMath::getPosOnLine(Vec2(0, 0), stickPos - originalPos, 1.0f);
}

void Joystick::_update()
{
    const auto isInside = [&](const Vec2& pos) -> bool {
        const float distance = MyMath::distance(pos, originalPos);
        return (distance <= touchRadius);
    };

    if (!isCatched) {
        auto touch = TouchesPool::instance->getNearestWithStartPos(originalPos);
        if (touch && isInside(touch->getStartLocation())) {
            startTouch(touch);
        }
        stick->setPosition(originalPos);
    } else {
        updateTouch();
    }
}

void Joystick::startTouch(Touch* touch)
{
    catchedTouch = touch;
    isCatched = true;

    //�������Ƴ�ʱ�Ĳ���
    const auto removeCallBack = [&](Touch*) {
        isCatched = false;
        catchedTouch = nullptr;
        //���Ƹ˵���Ч����0.3s�ĵ���Ч��
        const auto act = FadeOut::create(0.3f);
        const auto run = EaseOut::create(act, 1.5);
        effect->stopAllActions();
        effect->runAction(run);
    };
    TouchesPool::instance->registTouch(touch, removeCallBack);

    //���Ƹ˵���Ч����0.3s�ĵ���Ч��
    const auto act = FadeIn::create(0.3f);
    const auto run = EaseOut::create(act, 1.5f);
    effect->stopAllActions();
    effect->runAction(run);
}

void Joystick::updateTouch()
{
    const auto touchPos = catchedTouch->getLocation();
    const float distance = MyMath::distance(originalPos, touchPos);
    if (distance <= touchRadius) {
        stick->setPosition(touchPos);
    } else {
        stick->setPosition(
            MyMath::getPosOnLine(originalPos, touchPos, touchRadius));
    }
    effect->setRotation(MyMath::getRotation(originalPos, touchPos));
}