#ifndef __INT_VEC2_H__
#define __INT_VEC2_H__

#include "cocos2d.h"

/**
 *�����Ķ�ά����
 */

class iVec2 {
public:
    /**
     *����һ����ʼֵΪ0������
     */
    iVec2();

    /**
     *����һ������,������ʼֵ
     *@param xx: ������
     *@param yy: ������
     *@return ��
     */
    iVec2(int xx, int yy);

    /**
     *�����е�����ȥ��ʼ��������
     *@param copy: ���е�����
     *@return ��
     */
    iVec2(const iVec2& copy);

    /**
     *�ø�������ȥ��ʼ��������
     *@param copy: ���еĸ�������
     *@return ��
     */
    iVec2(const cocos2d::Vec2& copy);

    /**
     *��������
     */
    ~iVec2();

    /**
     *�����������
     *@param v: ��һ������
     *@return ��ӵĽ��
     */
    inline iVec2 operator+(const iVec2& v) const;

    /**
     *��������ֵ���ϸ���������ֵ
     *@param v: ��һ������
     *@return ���Ӻ�Ĵ�����
     */
    inline iVec2& operator+=(const iVec2& v);

    /**
     *�����������
     * @param v: ��һ������
     * @return ����Ľ��
     */
    inline iVec2 operator-(const iVec2& v) const;

    /**
     *��������ֵ��������������ֵ
     *@param v: ��һ������
     *@return ������Ĵ�����
     */
    inline iVec2& operator-=(const iVec2& v);

    /**
     *������x,y��ֵͬʱȡ�෴��
     *@param ��
     *@return ȡ�෴���������
     */
    inline iVec2 operator-() const;

    /**
     *�ж����������Ƿ����
     *@param v: ��һ������
     *@return �Ƿ����
     */
    inline bool operator==(const iVec2& v) const;

    /**
     *�ж����������Ƿ����
     *@param v: ��һ������
     *@return �Ƿ����
     */
    inline bool operator!=(const iVec2& v) const;

public:
    // ������
    int x;
    // ������
    int y;
};

#include "iVec2.inl"

#endif
