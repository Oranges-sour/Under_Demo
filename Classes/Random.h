#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <memory>
#include <random>
#include <vector>

#include "assert.h"

using rand_engine = std::default_random_engine;

class rand_bool;
class rand_int;
class rand_float;

class Random {
public:
    Random() {
        seed = rd();
        e.seed(seed);
    }
    Random(unsigned int seed) { e.seed(seed); }

    static Random* getDefault() { return defaultEngine; }

    unsigned int getSeed() { return seed; }

    rand_engine& getEngine() { return e; }

private:
    static Random* defaultEngine;

    unsigned int seed;
    rand_engine e;
    std::random_device rd;
};

class rand_int {
public:
    rand_int(int min, int max) {
        r = std::make_unique<std::uniform_int_distribution<int>>(min, max);
    }
    ~rand_int() {}
    int operator()() const {
        auto& engine = Random::getDefault()->getEngine();
        return (*r)(engine);
    }
    int operator()(Random& random) const {
        auto& engine = random.getEngine();
        return (*r)(engine);
    }

private:
    std::unique_ptr<std::uniform_int_distribution<int>> r;
};

class rand_float {
public:
    rand_float(float min, float max) {
        r = std::make_unique<std::uniform_real_distribution<float>>(min, max);
    }
    ~rand_float() {}
    float operator()() const {
        auto& engine = Random::getDefault()->getEngine();
        return (*r)(engine);
    }
    float operator()(Random& random) const {
        auto& engine = random.getEngine();
        return (*r)(engine);
    }

private:
    std::unique_ptr<std::uniform_real_distribution<float>> r;
};

class rand_bool {
public:
    rand_bool() {
        r = std::make_unique<std::uniform_int_distribution<int>>(0, 1);
    }
    ~rand_bool() {}
    bool operator()() const {
        auto& engine = Random::getDefault()->getEngine();
        return static_cast<bool>((*r)(engine));
    }
    bool operator()(Random& random) const {
        auto& engine = random.getEngine();
        return static_cast<bool>((*r)(engine));
    }

private:
    std::unique_ptr<std::uniform_int_distribution<int>> r;
};

template <class T>
class RandWithRate {
public:
    struct Elem;
    RandWithRate(const std::vector<Elem>& set) {
        this->set = set;
        check();
    }
    ~RandWithRate() {}
    struct Elem {
        T value;
        unsigned rate;
    };
    T operator()();
    T operator()(Random& random);

private:
    void check();

private:
    std::vector<Elem> set;
};

template <class T>
void RandWithRate<T>::check() {
    int percent = 0;
    for (size_t x = 0; x < set.size(); ++x) {
        percent += set[x].rate;
    }
    // 总百分比必须是100
    assert(percent == 100);
}

template <class T>
inline T RandWithRate<T>::operator()() {
    static rand_int rand(1, 100);
    int randPercent = rand();
    int nowPercent = 0;
    /*方法:
    先随机一个1~100的数字
    将所有param传进的几率进行相加,
    如果加了之后刚好在随机出的数字区间内
    就随机到了
    */
    for (size_t x = 0; x < set.size(); ++x) {
        nowPercent += set[x].rate;
        if (randPercent <= nowPercent) {
            return set[x].value;
        }
    }

    return T();
}

template <class T>
inline T RandWithRate<T>::operator()(Random& random) {
    static rand_int rand(1, 100);
    int randPercent = rand(random);
    int nowPercent = 0;
    /*方法:
    先随机一个1~100的数字
    将所有param传进的几率进行相加,
    如果加了之后刚好在随机出的数字区间内
    就随机到了
    */
    for (size_t x = 0; x < set.size(); ++x) {
        nowPercent += set[x].rate;
        if (randPercent <= nowPercent) {
            return set[x].value;
        }
    }

    return T();
}
#endif
