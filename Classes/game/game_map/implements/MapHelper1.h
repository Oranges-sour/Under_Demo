#ifndef __MAP_HELPER_1_H__
#define __MAP_HELPER_1_H__

#include "game/game_map/GameMap.h"

class MapHelper1 : public MapHelperComponent {
public:
    virtual iVec2 convertInMap(const Vec2& pos);
};

#endif
