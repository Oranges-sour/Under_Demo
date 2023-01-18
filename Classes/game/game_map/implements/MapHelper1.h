#ifndef __MAP_HELPER_1_H__
#define __MAP_HELPER_1_H__

#include "game/game_map/GameMap.h"

class MapHelperComponent1 : public MapHelperComponent {
public:
    virtual iVec2 convert_in_map(const Vec2& pos);
};

#endif
