#ifndef __INT_VEC2_H__
#define __INT_VEC2_H__

#include "cocos2d.h"

/**
 *整数的二维向量
 */

class iVec2 {
public:
    /**
     *创建一个初始值为0的向量
     */
    iVec2();

    /**
     *创建一个向量,给定初始值
     *@param xx: 横坐标
     *@param yy: 纵坐标
     *@return 无
     */
    iVec2(int xx, int yy);

    /**
     *用现有的向量去初始化此向量
     *@param copy: 现有的向量
     *@return 无
     */
    iVec2(const iVec2& copy);

    /**
     *用浮点向量去初始化此向量
     *@param copy: 现有的浮点向量
     *@return 无
     */
    iVec2(const cocos2d::Vec2& copy);

    /**
     *析构函数
     */
    ~iVec2();

    /**
     *两个向量相加
     *@param v: 另一个加数
     *@return 相加的结果
     */
    inline iVec2 operator+(const iVec2& v) const;

    /**
     *此向量的值加上给定向量的值
     *@param v: 另一个加数
     *@return 增加后的此向量
     */
    inline iVec2& operator+=(const iVec2& v);

    /**
     *两个向量相减
     * @param v: 另一个减数
     * @return 相减的结果
     */
    inline iVec2 operator-(const iVec2& v) const;

    /**
     *此向量的值减掉给定向量的值
     *@param v: 另一个减数
     *@return 减掉后的此向量
     */
    inline iVec2& operator-=(const iVec2& v);

    /**
     *向量的x,y的值同时取相反数
     *@param 无
     *@return 取相反数后的向量
     */
    inline iVec2 operator-() const;

    /**
     *判断两个向量是否相等
     *@param v: 另一个向量
     *@return 是否相等
     */
    inline bool operator==(const iVec2& v) const;

    /**
     *判断两个向量是否不相等
     *@param v: 另一个向量
     *@return 是否不相等
     */
    inline bool operator!=(const iVec2& v) const;

public:
    // 横坐标
    int x;
    // 纵坐标
    int y;
};

#include "iVec2.inl"

#endif
