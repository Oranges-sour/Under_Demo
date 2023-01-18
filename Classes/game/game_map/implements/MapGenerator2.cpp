#include "MapGenerator2.h"

void MapGenerator2::init(unsigned seed) {}

void MapGenerator2::generate(int w, int h, MapTile& map) {
    const auto air = MapTileType::air;
    const auto dirt = MapTileType::dirt;
    const auto grass = MapTileType::grass;
    const auto rock = MapTileType::rock;

    for (int i = 1; i <= w; ++i) {
        for (int j = 1; j <= h; ++j) {
            map[i][j] = air;
        }
    }

    map[2][3] = dirt;
    map[3][3] = dirt;
    map[3][2] = dirt;
}