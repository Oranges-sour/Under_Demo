#include "MapPreRenderer1.h"

void MapPreRendererComponent1::init(MapTile* map, unsigned int seed) {
    this->_map = map;
    x_max = map->w / 16;
    x_now = 0;

    y_max = map->h / 16;
    y_now = 0;

    _random = make_shared<Random>(seed);

    mark1.resize((map->w + 1) * (map->h + 1), false);
    mark.resize((map->w + 1) * (map->h + 1), false);
}

bool MapPreRendererComponent1::isPreRenderFinish() {
    return _isPreRenderFinish;
}

void MapPreRendererComponent1::preRender() {
    if (_isPreRenderFinish) {
        return;
    }
    if (x_now == x_max && y_now == y_max) {
        _isPreRenderFinish = true;
        return;
    }

    MapArea area;
    area.x = x_now * 16 + 1;
    area.y = y_now * 16 + 1;
    area.w = 16;
    area.h = 16;
    auto tex = this->render(area);
    // 防止被内存池回收
    tex->retain();
    textures.push_back({area, tex});

    y_now += 1;
    if (y_now >= y_max) {
        y_now = 0;
        x_now += 1;
    }

    if (x_now == x_max) {
        _isPreRenderFinish = true;
    }
}

Size MapPreRendererComponent1::afterPreRender(Node* target) {
    for (auto& it : textures) {
        auto& area = it.first;
        auto tex = it.second;

        auto sp = Sprite::createWithTexture(tex);
        sp->setScaleY(-1);
        sp->setAnchorPoint(Vec2(0, 1));
        sp->setPosition(Vec2((area.x - 1) * 64, (area.y - 1) * 64));
        target->addChild(sp);

        // 预渲染retain了一次，现在要release掉
        tex->release();
    }

    return Size(64 * _map->w, 64 * _map->h);
}

Texture2D* MapPreRendererComponent1::render(const MapArea& area) {
    const int pixleW = 64 * area.w;
    const int pixleH = 64 * area.h;

    const auto air = MapTileType::air;
    const auto dirt = MapTileType::dirt;
    const auto grass = MapTileType::grass;
    const auto rock = MapTileType::rock;

    /*地图掩码:
     * #############
     * # 1 # 2 # 3 #
     * #############
     * # 8 # A # 4 #
     * #############
     * # 7 # 6 # 5 #
     * #############
     *
     * 一个目标格子A，周围格子的对应标号
     * 周围一圈格子对应了 uchar 的8位
     * 8765 4321   标号
     * 0000 0000   uchar(bit)
     *
     * uchar的每一位 0代表空气，1代表有块
     * 由此来方便生成地图快的圆角遮罩
     *
     * eg：左上角圆角要求 1 2 8为空气，则使用 1000 0011（空气格子为1）
     * 与地图掩码进行 与 运算，如果结果为 0，则代表对应区域全为
     * 空气，可以添加圆角
     */

    // 访问顺序经过精心设计，请勿更改
    const int dx[8] = {-1, 0, 1, 1, 1, 0, -1, -1};
    const int dy[8] = {1, 1, 1, 0, -1, -1, -1, 0};

    const auto inside = [&](int x, int y) {
        if (x >= 1 && x <= _map->w && y >= 1 && y <= _map->h) {
            return true;
        }
        return false;
    };

    vector<Node*> sps;
    for (int x = area.x, xx = 0; x <= area.x + area.w - 1; x += 1, xx += 1) {
        for (int y = area.y, yy = 0; y <= area.y + area.h - 1;
             y += 1, yy += 1) {
            auto ty = (*_map)[x][y];
            if (ty == air) {
                continue;
            }

            int bit_mask = 0;

            for (int k = 0; k < 8; ++k) {
                int nx = x + dx[k];
                int ny = y + dy[k];
                if (!inside(nx, ny)) {
                    bit_mask = bit_mask | (1 << k);
                    continue;
                }
                auto ty = (*_map)[nx][ny];
                if (ty != air) {
                    bit_mask = bit_mask | (1 << k);
                }
            }

            Vec2 pos(64 * xx, 64 * yy);

            this->createTile(ty, bit_mask, pos, sps);

            this->createDecoration(x, y, ty, bit_mask, pos, area, sps);

            this->createTileMask(ty, bit_mask, pos, sps);
        }
    }

    auto rt = RenderTexture::create(pixleW, pixleH);
    rt->beginWithClear(0, 0, 0, 0);

    for (auto& it : sps) {
        it->visit();
    }

    rt->end();

    return rt->getSprite()->getTexture();
}

void MapPreRendererComponent1::createTile(MapTileType type, int bit_mask,
                                          const Vec2& pos, vector<Node*>& dst) {
    static const map<MapTileType, string> sprite_frame{
        {dirt, "game_map_tile_dirt.png"},
        {grass, "game_map_tile_grass.png"},
        {rock, "game_map_tile_rock.png"}};

    // 创建地图块
    const auto& sp = (*sprite_frame.find(type)).second;
    auto tile = Sprite::createWithSpriteFrameName(sp);
    tile->setAnchorPoint(Vec2(0, 0));
    tile->setPosition(pos);
    dst.push_back(tile);
}

void MapPreRendererComponent1::createDecoration(int x, int y, MapTileType type,
                                                int bit_mask, const Vec2& pos,
                                                const MapArea& area,
                                                vector<Node*>& dst) {
    createDec1(x, y, type, bit_mask, pos, area, dst);
    createDec2(x, y, type, bit_mask, pos, area, dst);
    createDec3(x, y, type, bit_mask, pos, area, dst);
    createDec4(x, y, type, bit_mask, pos, area, dst);
    createDec5(x, y, type, bit_mask, pos, area, dst);
    createDec6(x, y, type, bit_mask, pos, area, dst);
}

void MapPreRendererComponent1::createTileMask(MapTileType type, int bit_mask,
                                              const Vec2& pos,
                                              vector<Node*>& dst) {
    static const vector<pair<int, string>> sprite_mask{
        {0x83, "game_map_tile_cover0.png"},
        {0x0E, "game_map_tile_cover1.png"},
        {0x38, "game_map_tile_cover2.png"},
        {0xE0, "game_map_tile_cover3.png"}};

    // 创建地图快遮罩
    for (auto& it : sprite_mask) {
        // 验证成功，可以添加遮罩
        if ((bit_mask & it.first) == 0) {
            auto tile = Sprite::createWithSpriteFrameName(it.second);
            tile->setAnchorPoint(Vec2(0, 0));
            tile->setPosition(pos);
            tile->setBlendFunc({GL_DST_COLOR, GL_ZERO});
            dst.push_back(tile);
        }
    }
}

void MapPreRendererComponent1::createDec1(int x, int y, MapTileType type,
                                          int bit_mask, const Vec2& pos,
                                          const MapArea& area,
                                          vector<Node*>& dst) {
    static const vector<string> grass_decoration1{
        "game_map_tile_grass_decorate_1.png",
        "game_map_tile_grass_decorate_2.png",
        "game_map_tile_grass_decorate_3.png"};

    rand_int r1(0, 2);

    // 创建装饰
    if (type == grass && (bit_mask & 0x02) == 0 /*自己的上方是空气*/ &&
        (bit_mask & 0x88) == 0x88 /*自己左右两边有块*/) {
        auto g =
            Sprite::createWithSpriteFrameName(grass_decoration1[r1(*_random)]);
        g->setAnchorPoint(Vec2(0, 0));
        g->setPosition(pos + Vec2(0, 64));
        dst.push_back(g);
    }
}

void MapPreRendererComponent1::createDec2(int x, int y, MapTileType type,
                                          int bit_mask, const Vec2& pos,
                                          const MapArea& area,
                                          vector<Node*>& dst) {
    const int dx[] = {1, 2};
    const int dy[] = {0, 0};

    const int dx1[] = {0, 0, 0, 1, 1, 1, 2, 2, 2};
    const int dy1[] = {1, 2, 3, 1, 2, 3, 1, 2, 3};

    const auto inside = [&](int x, int y) {
        if (x >= area.x && x <= area.x + area.w - 1 && y >= area.y &&
            y <= area.y + area.h - 1) {
            return true;
        }
        return false;
    };

    static const vector<string> grass_decoration2{
        "game_map_tile_grass_decorate_4.png",
        "game_map_tile_grass_decorate_5.png"};

    rand_int r2(0, 1);

    if (type == grass && mark[y * _map->w + x] == false) {
        bool all_grass = true;
        // 大块草的装饰
        for (int k = 0; k < 2; ++k) {
            int nx = x + dx[k];
            int ny = y + dy[k];
            if (!inside(nx, ny)) {
                all_grass = false;
                break;
            }
            if ((*_map)[nx][ny] != grass) {
                all_grass = false;
                break;
            }

            mark[ny * _map->w + nx] = true;
        }
        if (all_grass) {
            bool all_air = true;
            for (int k = 0; k < 9; ++k) {
                int nx = x + dx1[k];
                int ny = y + dy1[k];
                if (!inside(nx, ny)) {
                    all_air = false;
                    break;
                }
                if ((*_map)[nx][ny] != air) {
                    all_air = false;
                    break;
                }
            }
            if (all_air) {
                auto g = Sprite::createWithSpriteFrameName(
                    grass_decoration2[r2(*_random)]);
                g->setAnchorPoint(Vec2(0, 0));
                g->setPosition(pos + Vec2(0, 64));
                dst.push_back(g);
            }
        }
    }
}

void MapPreRendererComponent1::createDec3(int x, int y, MapTileType type,
                                          int bit_mask, const Vec2& pos,
                                          const MapArea& area,
                                          vector<Node*>& dst) {
    const auto inside = [&](int x, int y) {
        if (x >= area.x && x <= area.x + area.w - 1 && y >= area.y &&
            y <= area.y + area.h - 1) {
            return true;
        }
        return false;
    };

    static const vector<string> grass_decoration3{
        "game_map_tile_grass_decorate_6.png"};

    const int dx2[] = {1, 2};
    const int dy2[] = {0, -1};

    const int dx3[] = {0, 0, 1, 1};
    const int dy3[] = {-1, -2, -1, -2};

    if (type == dirt && mark1[y * _map->w + x] == false) {
        bool all_block = true;
        for (int k = 0; k < 2; ++k) {
            int nx = x + dx2[k];
            int ny = y + dy2[k];
            if (!inside(nx, ny) || mark1[ny * _map->w + nx]) {
                all_block = false;
                break;
            }
            if ((*_map)[nx][ny] == air) {
                all_block = false;
                break;
            }
        }
        if (all_block) {
            bool all_air = true;
            for (int k = 0; k < 4; ++k) {
                int nx = x + dx3[k];
                int ny = y + dy3[k];
                if (!inside(nx, ny)) {
                    all_air = false;
                    break;
                }
                if ((*_map)[nx][ny] != air) {
                    all_air = false;
                    break;
                }
            }
            if (all_air) {
                mark1[y * _map->w + x] = true;
                mark1[y * _map->w + x + 1] = true;

                auto g =
                    Sprite::createWithSpriteFrameName(grass_decoration3[0]);
                g->setAnchorPoint(Vec2(0, 1));
                g->setPosition(pos);
                dst.push_back(g);
            }
        }
    }
}

void MapPreRendererComponent1::createDec4(int x, int y, MapTileType type,
                                          int bit_mask, const Vec2& pos,
                                          const MapArea& area,
                                          vector<Node*>& dst) {
    const auto inside = [&](int x, int y) {
        if (x >= area.x && x <= area.x + area.w - 1 && y >= area.y &&
            y <= area.y + area.h - 1) {
            return true;
        }
        return false;
    };

    static const vector<string> grass_decoration3{
        "game_map_tile_grass_decorate_6.png"};

    const int dx2[] = {1, -1};
    const int dy2[] = {0, -1};

    const int dx3[] = {0, 0, 1, 1};
    const int dy3[] = {-1, -2, -1, -2};

    if (type == dirt && mark1[y * _map->w + x] == false) {
        bool all_block = true;
        for (int k = 0; k < 2; ++k) {
            int nx = x + dx2[k];
            int ny = y + dy2[k];

            if (!inside(nx, ny) || mark1[ny * _map->w + nx]) {
                all_block = false;
                break;
            }
            if ((*_map)[nx][ny] == air) {
                all_block = false;
                break;
            }
        }
        if (all_block) {
            bool all_air = true;
            for (int k = 0; k < 4; ++k) {
                int nx = x + dx3[k];
                int ny = y + dy3[k];
                if (!inside(nx, ny)) {
                    all_air = false;
                    break;
                }
                if ((*_map)[nx][ny] != air) {
                    all_air = false;
                    break;
                }
            }
            if (all_air) {
                mark1[y * _map->w + x] = true;
                mark1[y * _map->w + (x - 1)] = true;

                auto g =
                    Sprite::createWithSpriteFrameName(grass_decoration3[0]);
                g->setScaleX(-1);
                g->setAnchorPoint(Vec2(0, 1));
                g->setPosition(pos + Vec2(128, 0));
                dst.push_back(g);
            }
        }
    }
}

void MapPreRendererComponent1::createDec5(int x, int y, MapTileType type,
                                          int bit_mask, const Vec2& pos,
                                          const MapArea& area,
                                          vector<Node*>& dst) {
    const auto inside = [&](int x, int y) {
        if (x >= area.x && x <= area.x + area.w - 1 && y >= area.y &&
            y <= area.y + area.h - 1) {
            return true;
        }
        return false;
    };

    static const vector<string> grass_decoration{
        "game_map_tile_grass_decorate_7.png",
        "game_map_tile_grass_decorate_8.png"};

    const int dx[] = {1};
    const int dy[] = {0};

    const int dx1[] = {0, 1, 0, 1};
    const int dy1[] = {-1, -1, -2, -2};

    const int dx2[] = {0, 1, 0, 1, 0, 1};
    const int dy2[] = {-1, -1, -2, -2, -3, -3};

    if ((*_map)[x][y] != air && mark1[y * _map->w + x] == false) {
        bool step1 = true;
        for (int k = 0; k < 1; ++k) {
            int nx = x + dx[k];
            int ny = y + dy[k];
            if (!inside(nx, ny) || mark1[ny * _map->w + nx]) {
                step1 = false;
                break;
            }
            if ((*_map)[nx][ny] == air) {
                step1 = false;
            }
        }

        if (step1) {
            int rand_range = 0;
            bool suc = true;
            for (int k = 0; k < 4; ++k) {
                int nx = x + dx1[k];
                int ny = y + dy1[k];
                if (!inside(nx, ny)) {
                    suc = false;
                    break;
                }
                if ((*_map)[nx][ny] != air) {
                    suc = false;
                    break;
                }
            }
            if (suc) {
                bool suc1 = true;
                rand_range += 1;
                for (int k = 0; k < 6; ++k) {
                    int nx = x + dx2[k];
                    int ny = y + dy2[k];
                    if (!inside(nx, ny)) {
                        suc1 = false;
                        break;
                    }
                    if ((*_map)[nx][ny] != air) {
                        suc1 = false;
                        break;
                    }
                }
                rand_int r1(1, 10);
                if (suc1) {
                    // 有20%几率不生成
                    if (r1(*_random) <= 2) {
                        return;
                    }
                    mark1[y * _map->w + x] = true;
                    mark1[y * _map->w + (x + 1)] = true;

                    rand_range += 1;

                    rand_int r(0, rand_range - 1);
                    auto g = Sprite::createWithSpriteFrameName(
                        grass_decoration[r(*_random)]);
                    g->setAnchorPoint(Vec2(0, 1));
                    g->setPosition(pos);
                    dst.push_back(g);
                }
            }
        }
    }
}

void MapPreRendererComponent1::createDec6(int x, int y, MapTileType type,
                                          int bit_mask, const Vec2& pos,
                                          const MapArea& area,
                                          vector<Node*>& dst) {
    const auto inside = [&](int x, int y) {
        if (x >= area.x && x <= area.x + area.w - 1 && y >= area.y &&
            y <= area.y + area.h - 1) {
            return true;
        }
        return false;
    };

    static const vector<string> grass_decoration{
        "game_map_tile_grass_decorate_9.png"};

    const int dx[] = {1, 2, 3};
    const int dy[] = {0, 0, 0};

    const int dx1[] = {0, 1, 2, 3, 0, 1, 2, 3};
    const int dy1[] = {-1, -1, -1, -1, -2, -2, -2, -2};
    if (type != air && mark1[y * _map->w + x] == false) {
        bool step1 = true;
        for (int k = 0; k < 3; ++k) {
            int nx = x + dx[k];
            int ny = y + dy[k];
            if (!inside(nx, ny)) {
                step1 = false;
                break;
            }
            if ((*_map)[nx][ny] == air || mark1[ny * _map->w + nx]) {
                step1 = false;
            }
        }
        if (step1) {
            bool suc = true;
            for (int k = 0; k < 8; ++k) {
                int nx = x + dx1[k];
                int ny = y + dy1[k];
                if (!inside(nx, ny)) {
                    suc = false;
                    break;
                }
                if ((*_map)[nx][ny] != air) {
                    suc = false;
                    break;
                }
            }
            if (suc) {
                mark1[y * _map->w + x] = true;
                mark1[y * _map->w + x + 1] = true;
                mark1[y * _map->w + x + 2] = true;
                mark1[y * _map->w + x + 3] = true;

                auto g = Sprite::createWithSpriteFrameName(grass_decoration[0]);
                g->setAnchorPoint(Vec2(0, 1));
                g->setPosition(pos);
                dst.push_back(g);
            }
        }
    }
}