#ifndef __MY_MATH_H__
#define __MY_MATH_H__

#include "cocos2d.h"
USING_NS_CC;
#include <cmath>

/**
 *ƽ��
 *@param t:Ҫ��ƽ������
 *@return t��ƽ��
 */
inline float POT(float t) { return t * t; }

constexpr float PI = 3.1415926f;

//���Ǻ���,�Ƕ�ģʽ
class DEG {
   public:
    static float tan(float d) { return tanf(CC_DEGREES_TO_RADIANS(d)); }
    static float sin(float d) { return sinf(CC_DEGREES_TO_RADIANS(d)); }
    static float cos(float d) { return cosf(CC_DEGREES_TO_RADIANS(d)); }

    static float atan(float d) { return CC_RADIANS_TO_DEGREES(atanf(d)); }
    static float asin(float d) { return CC_RADIANS_TO_DEGREES(asinf(d)); }
    static float acos(float d) { return CC_RADIANS_TO_DEGREES(acosf(d)); }
};

class MyMath {
   public:
    /**
    *��fromָ��toʱ,������Ҫ����ת�Ƕ�
    *@param from:��ʼ��
    *@param to:�յ�
    *@return ��ת�Ƕ�
    ��ת����:
    �����ҷ���Ϊ0��,˳ʱ�뷽���0~360��

                      270��
                       ��(y)
                       ��
                       |
       180�ȩ������੤������0��(360��)
                       ��    (x)
                       ��
                      90��
    */
    static float getRotation(const Vec2& from, const Vec2& to);

    /**
     *��һ����������һ������,���Ӻ��ܷ���cocos����ת����
     *@param nowRotation:ԭ����
     *@param addRotation:���Ӷ���
     *@return ���Ӻ�Ķ���
     */
    static float addRotation(const float nowRotation, const float addRotation);

    /**
     *�����ľ���
     *@param p1:��һ����
     *@param p2:�ڶ�����
     *@return ����
     */
    static float distance(const Vec2& p1, const Vec2& p2);

    /**
     *���ֱ���߶���,����ʼ�㿪ʼ����һ�γ��ȵ�����,��from��toΪ����
     *@param from:��ʼ��
     *@param to:����
     *@return ����
     */
    static Vec2 getPosOnLine(const Vec2& from, const Vec2& to, float distance);

    /**
     *�ж������������Ƿ����
     *@param num1:��һ����
     *@param num2:�ڶ�����
     *@param delta:�жϾ���(���������������С��������ֵ����Ϊ�����)
     *@return �Ƿ����
     */
    static bool float_equal(float num1, float num2, float delta = 0.000001f);

    /**
     *���һ������,��positionΪ���ĵ�,x��y��ƫ����������min��max֮��
     *@param position:���ĵ�
     *@param min:��Сƫ����
     *@param max:���ƫ����
     *@return
     */
    static Vec2 randPos(const Vec2& position, float min, float max);

    /**
     *��float��������һλ��int
     *@param num:С��
     *@return ��������������
     */
    static int floatRoundInt(float num);

    /**
     *��ʼ����Ļ����ƴ�С
     *@param size:��ƴ�С
     *@return ��
     */
    static void setScreenSize(const Size& size);

    /**
     *�����Ļ����ƴ�С
     *@return ��Ļ��С
     */
    static Size getScreenSize();

   private:
    static Size screenSize;
};

inline float MyMath::distance(const Vec2& p1, const Vec2& p2)
{
    return sqrt(POT(p1.x - p2.x) + POT(p1.y - p2.y));
}

inline bool MyMath::float_equal(float num1, float num2, float delta)
{
    return (abs(num1 - num2) <= delta);
}

inline int MyMath::floatRoundInt(float num)
{
    return static_cast<int>(num + 0.5f);
}

inline void MyMath::setScreenSize(const Size& size) { screenSize = size; }

inline Size MyMath::getScreenSize() { return screenSize; }

#endif