#include "TouchesPool.h"
#include "utility/math/MyMath.h"
#include <limits>

using namespace std;

TouchesPool* TouchesPool::_instance = new TouchesPool{};

TouchesPool::TouchesPool()
{
}

TouchesPool::~TouchesPool()
{
}

void TouchesPool::addToPool(Touch* touch)
{
	_pool.push_back(touch);
}

const vector<Touch*>& TouchesPool::getAllTouches()
{
	return _pool;
}

void TouchesPool::removeFromPool(Touch* touch)
{
	_pool.erase(remove(_pool.begin(), _pool.end(), touch), _pool.end());
	auto iter = _registed_touch.find(touch);
	if (iter != _registed_touch.end())
	{
		auto& callBacks = iter->second;
		for (auto& it : callBacks)
			it(touch);
		_registed_touch.erase(iter);
	}
}

void TouchesPool::removeAllTouches()
{
	for (auto& touch : _pool)
	{
		auto iter = _registed_touch.find(touch);
		if (iter != _registed_touch.end())
		{
			auto& callBacks = iter->second;
			for (auto& it : callBacks)
				it(touch);
		}
	}
	_pool.clear();
	_registed_touch.clear();
}

void TouchesPool::registTouch(Touch* touch, const function<void(Touch*)>& removeCallBack)
{
	auto iter = _registed_touch.find(touch);
	if (iter != _registed_touch.end())
		iter->second.push_back(removeCallBack);
	else
		_registed_touch.insert({ touch,{removeCallBack} });
}

Touch* TouchesPool::getNearest(const Vec2& pos)
{
	struct
	{
		float distance = numeric_limits<float>::max();
		Touch* touch = nullptr;
	}temp;

	for (auto it = _pool.begin(); it != _pool.end(); ++it)
	{
		auto& touch = *it;
		float _distance = MyMath::distance(touch->getLocation(), pos);
		if (_distance < temp.distance)
		{
			temp.distance = _distance;
			temp.touch = touch;
		}
	}
	return temp.touch;
}

Touch* TouchesPool::getNearestWithStartPos(const Vec2& pos)
{
	struct
	{
		float distance = numeric_limits<float>::max();
		Touch* touch = nullptr;
	}temp;

	for (auto it = _pool.begin(); it != _pool.end(); ++it)
	{
		auto& touch = *it;
		float _distance = MyMath::distance(touch->getStartLocation(), pos);
		if (_distance < temp.distance)
		{
			temp.distance = _distance;
			temp.touch = touch;
		}
	}
	return temp.touch;
}
