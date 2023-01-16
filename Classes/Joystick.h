#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

#include "cocos2d.h"
USING_NS_CC;
#include <string>

class Joystick : public Node {
public:
    /**
     *����ҡ��
     *@param backGround:ҡ�˵ı���
     *@param stick:ҡ��
     *@param effect:ҡ�˵���Ч
     *@return ҡ��
     */
    static Joystick* create(const std::string& backGround,
                            const std::string& stick,
                            const std::string& effect);

    /**
     *��ʼ��ҡ��
     *@param ��
     *@return ��
     */
    bool init(const std::string& backGround, const std::string& stick,
              const std::string& effect);

    /**
     *����ҡ������
     *@param position:����
     *@return ��
     */
    void setOriginalPosition(const Vec2& position);

    /**
     *���ҡ�˵�����ƶ�����,���������Ϊ 0~1
     *@param ��
     *@return ����ƶ�����
     */
    Vec2 getMoveVec();

    /**
     *������
     *@param ��
     *@return ��
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