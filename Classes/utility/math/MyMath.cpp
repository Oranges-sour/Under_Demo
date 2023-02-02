#include "MyMath.h"

#include "Random.h"

float MyMath::getRotation(const Vec2& from, const Vec2& to) {
    auto newTo = to;
    // 防止k无限大
    if (MyMath::fEq(from.x, to.x)) newTo.x -= 0.5;
    const float x = newTo.x - from.x;
    const float y = newTo.y - from.y;

    const float k = y / x;
    const float angle = abs(DEG::atan(k));

    float rotation = 0.0f;

    // 顺时针旋转为正角度
    // 旋转度数  0~360(顺时针

    // x轴上方
    if (from.y < newTo.y) {
        // 右上
        if (from.x < newTo.x) rotation = 360 - angle;
        // 左上
        else
            rotation = 180 + angle;
    }
    // x轴下方
    else {
        // 右下
        if (from.x < newTo.x) rotation = angle;
        // 左下
        else
            rotation = 180 - angle;
    }

    return rotation;
}

float MyMath::addRotation(const float nowRotation, const float addRotation) {
    if (addRotation > 0) {
        if (nowRotation + addRotation > 360)  // 转的过一圈了,回到0
            return nowRotation + addRotation - 360;
        else
            return nowRotation + addRotation;
    } else {
        if (nowRotation + addRotation < 0)  // 转的过一圈了,回到0
            return 360 + nowRotation + addRotation;
        else
            return nowRotation + addRotation;
    }
}

Vec2 MyMath::getPosOnLine(const Vec2& from, const Vec2& to, float distance) {
    auto newTo = to;
    // 防止k无限大
    if (MyMath::fEq(from.x, newTo.x, 0.01f)) newTo.x -= 0.5;
    const float k = (newTo.y - from.y) / (newTo.x - from.x);

    float x = distance / sqrt(POT(k) + 1);
    if (from.x > newTo.x) x = -x;
    const float y = k * x;
    return Vec2(from.x + x, from.y + y);
}

Vec2 MyMath::randPos(const Vec2& position, float min, float max,
                     Random& random) {
    auto result = position;
    rand_float r0(min, max);
    rand_bool r1;

    if (r1())
        result.x += r0(random);
    else
        result.x -= r0(random);

    if (r1())
        result.y += r0(random);
    else
        result.y -= r0(random);

    return result;
}