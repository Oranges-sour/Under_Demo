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
	static TouchesPool* _instance;
private:
	TouchesPool();
	~TouchesPool();
public:
	/**
	*添加触摸到池
	*@param touch:触摸指针
	*@return 无
	*/
	void addToPool(Touch* touch);

	/**
	*获得所有触摸
	*@param 无
	*@return 所有触摸
	*/
	const std::vector<Touch*>& getAllTouches();

	/**
	*从池中移除
	*@param touch:要移除的触摸
	*@return 无
	*/
	void removeFromPool(Touch* touch);

	/**
	*移除所有触摸
	*@param 无
	*@return 无
	*/
	void removeAllTouches();

	/**
	*注册池中已有的触摸的回调,在要删除触摸时会调用回调函数
	*@param touch:池中已有的触摸
	*@param removeCallBack:删除触摸时会调用的回调函数
	*@return 无
	*/
	void registTouch(Touch* touch, const std::function<void(Touch*)>& removeCallBack);

	/**
	*获得距目标坐标最近的触摸当前位置的触摸
	*@param pos:目标坐标
	*@return 最近的触摸
	*/
	Touch* getNearest(const Vec2& pos);

	/**
	*获得距目标坐标最近的触摸起始点的触摸
	*@param
	*@return
	*/
	Touch* getNearestWithStartPos(const Vec2& pos);
private:
	std::vector<Touch*> _pool;
	std::map<Touch*, std::vector<std::function<void(Touch*)>>> _registed_touch;
};

#endif