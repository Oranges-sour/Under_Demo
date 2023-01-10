#include "iVec2.h"

inline iVec2::iVec2() : x(0), y(0) {}

inline iVec2::iVec2(int xx, int yy) : x(xx), y(yy) {}

inline iVec2::iVec2(const iVec2& copy) {
    x = copy.x;
    y = copy.y;
}

inline iVec2::iVec2(const cocos2d::Vec2& copy) {
    x = static_cast<int>(copy.x);
    y = static_cast<int>(copy.y);
}

inline iVec2::~iVec2() {}

inline iVec2 iVec2::operator+(const iVec2& v) const {
    iVec2 result;
    result.x = x + v.x;
    result.y = y + v.y;
    return result;
}

inline iVec2& iVec2::operator+=(const iVec2& v) {
    x += v.x;
    y += v.y;
    return *this;
}

inline iVec2 iVec2::operator-(const iVec2& v) const {
    iVec2 result;
    result.x = x - v.x;
    result.y = y - v.y;
    return result;
}

inline iVec2& iVec2::operator-=(const iVec2& v) {
    x -= v.x;
    y -= v.y;
    return *this;
}

inline iVec2 iVec2::operator-() const {
    iVec2 result;
    result.x = -x;
    result.y = -y;
    return result;
}

inline bool iVec2::operator==(const iVec2& v) const {
    return x == v.x && y == v.y;
}

inline bool iVec2::operator!=(const iVec2& v) const {
    return x != v.x || y != v.y;
}
