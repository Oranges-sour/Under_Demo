#include "GameComponet.h"
#include "GameObject.h"

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