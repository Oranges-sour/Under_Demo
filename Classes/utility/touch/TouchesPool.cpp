#include "TouchesPool.h"
#include "MyMath.h"
#include <limits>

using namespace std;

TouchesPool* TouchesPool::instance = new TouchesPool{};

TouchesPool::TouchesPool()
{
}

TouchesPool::~TouchesPool()
{
}

void TouchesPool::addToPool(Touch* touch)
{
	pool.push_back(touch);
}

const vector<Touch*>& TouchesPool::getAllTouches()
{
	return pool;
}

void TouchesPool::removeFromPool(Touch* touch)
{
	pool.erase(remove(pool.begin(), pool.end(), touch), pool.end());
	auto iter = registedTouch.find(touch);
	if (iter != registedTouch.end())
	{
		auto& callBacks = iter->second;
		for (auto& it : callBacks)
			it(touch);
		registedTouch.erase(iter);
	}
}

void TouchesPool::removeAllTouches()
{
	for (auto& touch : pool)
	{
		auto iter = registedTouch.find(touch);
		if (iter != registedTouch.end())
		{
			auto& callBacks = iter->second;
			for (auto& it : callBacks)
				it(touch);
		}
	}
	pool.clear();
	registedTouch.clear();
}

void TouchesPool::registTouch(Touch* touch, const function<void(Touch*)>& removeCallBack)
{
	auto iter = registedTouch.find(touch);
	if (iter != registedTouch.end())
		iter->second.push_back(removeCallBack);
	else
		registedTouch.insert({ touch,{removeCallBack} });
}

Touch* TouchesPool::getNearest(const Vec2& pos)
{
	struct
	{
		float distance = numeric_limits<float>::max();
		Touch* touch = nullptr;
	}temp;

	for (auto it = pool.begin(); it != pool.end(); ++it)
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

	for (auto it = pool.begin(); it != pool.end(); ++it)
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
