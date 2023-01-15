#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

#include "cocos2d.h"
USING_NS_CC;
#include <string>

class Joystick : public Node {
public:
    /**
     *创建摇杆
     *@param backGround:摇杆的背景
     *@param stick:摇杆
     *@param effect:摇杆的特效
     *@return 摇杆
     */
    static Joystick* create(const std::string& backGround,
                            const std::string& stick,
                            const std::string& effect);

    /**
     *初始化摇杆
     *@param 略
     *@return 略
     */
    bool init(const std::string& backGround, const std::string& stick,
              const std::string& effect);

    /**
     *设置摇杆坐标
     *@param position:坐标
     *@return 无
     */
    void setOriginalPosition(const Vec2& position);

    /**
     *获得摇杆的相对移动坐标,横纵坐标均为 0~1
     *@param 无
     *@return 相对移动坐标
     */
    Vec2 getMoveVec();

    /**
     *主更新
     *@param 无
     *@return 无
     */
    void _update();

private:
    void setPosition(float, float) override {}
    void setPosition(const Vec2&) override {}

    void startTouch(Touch* touch);
    void updateTouch();

protected:
    Sprite* backGround = nullptr;
    Sprite* stick = nullptr;
    Sprite* effect = nullptr;

    Vec2 originalPos;
    bool isCatched = false;
    Touch* catchedTouch = nullptr;
    float touchRadius = 0;
};

#endif