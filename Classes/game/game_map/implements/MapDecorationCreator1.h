#ifndef __MAP_DECORATION_CREATOR_1_H__
#define __MAP_DECORATION_CREATOR_1_H__

#include <vector>
using namespace std;

#include "game/game_map/GameMap.h"
#include "utility/math/iVec2.h"

class MapDecorationCreator1 : public MapDecorationCreatorComponent {
public:
    virtual void generate(GameWorld* world, MapTile* map_tile,
                          unsigned int seed);

    void setDec2Pos(const vector<iVec2>& dec2_pos);

private:
    //生成光源水晶
    void generate_1(GameWorld* world, MapTile* map_tile, unsigned int seed);

     // 生成紫藤萝照明
    void generate_2(GameWorld* world, MapTile* map_tile, unsigned int seed);


    virtual void generate(GameWorld* world, MapTile* map_tile) override {}

private:
    vector<iVec2> dec1_pos;

    vector<iVec2> dec2_pos;
};

#endif