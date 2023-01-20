#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <memory>
#include <random>
#include <vector>

#include "assert.h"

using rand_engine = std::mt19937;

class rand_bool;
class rand_int;
class rand_float;

class Random {
public:
    Random() {
        seed = rd();
        e.seed(seed);
    }
    Random(unsigned int seed) {
        this->seed = seed;
        e.seed(seed);
    }

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
        //max+1 ֮��С������ȡ������֤[min, max]֮���������һ��
        r = std::make_shared<std::uniform_real_distribution<float>>(min,
                                                                    max + 1);
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
    std::shared_ptr<std::uniform_real_distribution<float>> r;
};

class rand_float {
public:
    rand_float(float min, float max) {
        r = std::make_shared<std::uniform_real_distribution<float>>(min, max);
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
    std::shared_ptr<std::uniform_real_distribution<float>> r;
};

class rand_bool {
public:
    rand_bool() {
        r = std::make_shared<std::uniform_real_distribution<float>>(-1, 1);
    }
    ~rand_bool() {}
    bool operator()() const {
        auto& engine = Random::getDefault()->getEngine();
        return static_cast<bool>((*r)(engine));
    }
    bool operator()(Random& random) const {
        auto& engine = random.getEngine();
        float f = (*r)(engine);
        if (f < 0) {
            return false;
        }
        return true;
    }

private:
    std::shared_ptr<std::uniform_real_distribution<float>> r;
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
        unsigned int rate;
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
    // �ܰٷֱȱ�����100
    assert(percent == 100);
}

template <class T>
inline T RandWithRate<T>::operator()() {
    static rand_int rand(1, 100);
    int randPercent = rand();
    int nowPercent = 0;
    /*����:
    �����һ��1~100������
    ������param�����ļ��ʽ������,
    �������֮��պ��������������������
    ���������
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
    /*����:
    �����һ��1~100������
    ������param�����ļ��ʽ������,
    �������֮��պ��������������������
    ���������
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