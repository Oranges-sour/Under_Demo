#ifndef __TOUCHES_POOL_H__
#define __TOUCHES_POOL_H__

#include <vector>
#include <functional>
#include <map>

#include "cocos2d.h"
USING_NS_CC;

class TouchesPool
{
public:
	static TouchesPool* instance;
private:
	TouchesPool();
	~TouchesPool();
public:
	/**
	*��Ӵ�������
	*@param touch:����ָ��
	*@return ��
	*/
	void addToPool(Touch* touch);

	/**
	*������д���
	*@param ��
	*@return ���д���
	*/
	const std::vector<Touch*>& getAllTouches();

	/**
	*�ӳ����Ƴ�
	*@param touch:Ҫ�Ƴ��Ĵ���
	*@return ��
	*/
	void removeFromPool(Touch* touch);

	/**
	*�Ƴ����д���
	*@param ��
	*@return ��
	*/
	void removeAllTouches();

	/**
	*ע��������еĴ����Ļص�,��Ҫɾ������ʱ����ûص�����
	*@param touch:�������еĴ���
	*@param removeCallBack:ɾ������ʱ����õĻص�����
	*@return ��
	*/
	void registTouch(Touch* touch, const std::function<void(Touch*)>& removeCallBack);

	/**
	*��þ�Ŀ����������Ĵ�����ǰλ�õĴ���
	*@param pos:Ŀ������
	*@return ����Ĵ���
	*/
	Touch* getNearest(const Vec2& pos);

	/**
	*��þ�Ŀ����������Ĵ�����ʼ��Ĵ���
	*@param
	*@return
	*/
	Touch* getNearestWithStartPos(const Vec2& pos);
private:
	std::vector<Touch*> pool;
	std::map<Touch*, std::vector<std::function<void(Touch*)>>> registedTouch;
};

#endif