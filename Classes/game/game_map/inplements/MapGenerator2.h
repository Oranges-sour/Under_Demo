#ifndef __MAP_GENERATOR_2_H__
#define __MAP_GENERATOR_2_H__

#include "game/game_map/GameMap.h"

class MapGeneratorComponent2 : public MapGeneratorComponent {
public:
    void init(unsigned seed);

    virtual void generate(int w, int h, MapTile& map);

private:
    shared_ptr<Random> random;
};

#endif