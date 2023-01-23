#include "RefLineLayer.h"

bool RefLineLayer::init() {
    if (!Layer::init()) {
        return false;
    }

    auto vs = Director::getInstance()->getVisibleSize();

    const Color4F color(Color4B(8, 241, 235, 255));

    auto draw = DrawNode::create();
    this->addChild(draw);

    // ÖÐÏß

    draw->drawLine(Vec2(0, vs.height / 2), Vec2(vs.width, vs.height / 2),
                   color);

    draw->drawLine(Vec2(vs.width / 2, 0), Vec2(vs.width / 2, vs.height), color);

    //
    draw->drawLine(Vec2(0, vs.height - 25), Vec2(vs.width, vs.height - 25),
                   color);

    draw->drawLine(Vec2(0, 25), Vec2(vs.width, 25), color);

    //
    draw->drawLine(Vec2(450, 0), Vec2(450, vs.height), color);

    draw->drawLine(Vec2(vs.width - 450, 0), Vec2(vs.width - 450, vs.height),
                   color);

    //
    draw->drawLine(Vec2(100, 0), Vec2(100, vs.height), color);

    draw->drawLine(Vec2(vs.width - 100, 0), Vec2(vs.width - 100, vs.height),
                   color);

    //
    draw->drawCircle(Vec2(100, 100), 100, 0, 20, false, color);
    draw->drawCircle(Vec2(100, vs.height - 100), 100, 0, 20, false, color);
    draw->drawCircle(Vec2(vs.width - 100, 100), 100, 0, 20, false, color);
    draw->drawCircle(Vec2(vs.width - 100, vs.height - 100), 100, 0, 20, false,
                     color);

    return true;
}
