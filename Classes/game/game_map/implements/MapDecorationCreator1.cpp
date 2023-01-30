#include "MapDecorationCreator1.h"

#include "game/game_object/implements/mpa_decoration/decoration_1/Decoration1.h"
#include "game/game_object/implements/mpa_decoration/decoration_2/Decoration2.h"
#include "utility/math/Random.h"

void MapDecorationCreator1::generate(GameWorld* world, MapTile* map_tile,
                                     unsigned int seed) {
    generate_1(world, map_tile, seed);
    generate_2(world, map_tile, seed);
}

void MapDecorationCreator1::setDec2Pos(const vector<iVec2>& dec2_pos) {
    this->dec2_pos = dec2_pos;
}

void MapDecorationCreator1::generate_1(GameWorld* world, MapTile* map_tile,
                                       unsigned int seed) {
    auto& map = *map_tile;

    const auto inside = [&](int x, int y) {
        if (x >= 1 && x <= map._w && y >= 1 && y <= map._h) {
            return true;
        }
        return false;
    };

    // 先找出所有符合的
    for (int x = 2; x <= map._w - 1; ++x) {
        for (int y = 2; y <= map._h - 1; ++y) {
            auto t = map[x][y];
            if (t == MapTileType::grass) {
                int nx = x;
                int ny = y + 1;
                if (!inside(nx, ny)) {
                    continue;
                }

                auto nt = map[nx][ny];
                if (nt == MapTileType::air) {
                    dec1_pos.push_back({x, y});
                }
            }
        }
    }

    Random random(seed);

    rand_int r(5, 10);
    for (int i = 0; i < dec1_pos.size();) {
        auto p = dec1_pos[i];
        // 创建
        MapDecoration1::create(world,
                               Vec2((p.x - 1) * 64 + 32, (p.y) * 64 + 32));

        i += r(random);
    }
}

void MapDecorationCreator1::generate_2(GameWorld* world, MapTile* map_tile,
                                       unsigned int seed) {
    Random random(seed);

    rand_int r(1, 1);

    for (int i = 0; i < dec2_pos.size();) {
        auto p = dec2_pos[i];
        // 创建
        MapDecoration2::create(world,
                               Vec2((p.x - 1) * 64 + 64, (p.y - 1) * 64 - 80));
        i += r(random);
    }
}
