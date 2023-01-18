#ifndef __MAP_GENERATOR_1_H__
#define __MAP_GENERATOR_1_H__

#include "game/game_map/GameMap.h"


class MapGeneratorComponent1 : public MapGeneratorComponent {
public:
    void init(unsigned int seed);

    virtual void generate(int w, int h, MapTile& map);

private:
    shared_ptr<Random> random;
};

#endif