#include "MapHelper1.h"

iVec2 MapHelper1::convert_in_map(const Vec2& pos) {
    const auto convert_in_map = [&](float num) {
        int k = ((int)num) / 64;
        if (k * 64 < num) {
            k += 1;
        }
        return k;
    };

    return iVec2(convert_in_map(pos.x), convert_in_map(pos.y));
}
