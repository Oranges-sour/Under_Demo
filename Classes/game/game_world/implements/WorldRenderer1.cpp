#include "WorldRenderer1.h"

#include "game/game_map/GameMap.h"
#include "utility/math/MyMath.h"

void GameWorldRenderer1::init(Node* target) {
    this->light = Sprite::create();

    const auto visibleSize = Director::getInstance()->getVisibleSize();
    light->setPosition(visibleSize / 2);
    light->setScaleY(-1);

    target->addChild(light);

    this->render = RenderTexture::create(visibleSize.width, visibleSize.height);
    this->render->retain();

    const vector<string> light_name{"light_1.png", "light_2.png",
                                    "light_3.png"};
    for (int i = 0; i < 3; ++i) {
        lights[i].resize(50);
        for (auto& it : lights[i]) {
            it = Sprite::createWithSpriteFrameName(light_name[i]);
            it->retain();
        }
    }
}

void GameWorldRenderer1::release() {
    if (render) {
        render->release();
    }
    for (int i = 0; i < 3; ++i) {
        for (auto& it : lights[i]) {
            it->release();
        }
    }
}

void GameWorldRenderer1::update(const Vec2& left_bottom, const Size& size,
                                GameWorld* gameworld) {
    auto gameMap = gameworld->getGameMap();
    auto ilb = gameMap->getMapHelper()->convertInMap(left_bottom);

    auto isize =
        gameMap->getMapHelper()->convertInMap(Vec2(size.width, size.height));

    auto& quad = gameworld->getGameObjects();

    int cnt[3] = {0, 0, 0};
    // 边缘扩大一点，保证光源能够完整绘制
    quad.visitInRect(
        {ilb.x - 5, ilb.y + isize.y + 5}, {ilb.x + isize.x + 5, ilb.y - 5},
        [&](const iVec2& coor, GameObject* object) {
            auto& pos = object->getPosition();
            auto& lig = object->getAllWorldLight();

            for (auto& it : lig) {
                auto& li = it.second;
                int k = li.type - 1;  // type从1开始，对应下标要减一

                if (cnt[k] >= 50) {
                    continue;
                }

                auto& light_sp = lights[k][cnt[k]];
                light_sp->setPosition(pos - left_bottom + li.offset);
                light_sp->setScale(li.radius / 100.0f);
                light_sp->setColor(li.lightColor);
                light_sp->setOpacity(255 * li.opacity);

                cnt[k] += 1;
            }
        });
    // 0.2 可以让背景不是全黑

    render->beginWithClear(0.1, 0.1, 0.1, 1);
    for (int k = 0; k < 3; ++k) {
        for (int i = 0; i < cnt[k]; ++i) {
            lights[k][i]->visit();
        }
    }
    render->end();

    light->setSpriteFrame(render->getSprite()->getSpriteFrame());
    light->setBlendFunc({GL_DST_COLOR, GL_ZERO});
}

Vec2 GameWorldRenderer1::calcuCameraSpeed(const Vec2& current_pos,
                                            const Vec2& target_pos) {
    // 更新摄像机坐标
    const auto calcuSpeed = [](float x) -> float {
        return (x * x) / (12 * (x + 3));
    };
    auto del = target_pos - current_pos;
    float dis = del.distance(Vec2(0, 0));
    float speed = calcuSpeed(dis);
    auto move_pos = MyMath::getPosOnLine(Vec2(0, 0), del, speed);

    return move_pos;
}