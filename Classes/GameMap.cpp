#include "GameMap.h"

#include "GameObject.h"
#include "GameWorld.h"
#include "PhysicsShapeCache.h"

void GameMap::init(shared_ptr<MapGeneratorComponent> mapGenerator,
                   shared_ptr<MapHelperComponent> mapHelper,
                   shared_ptr<MapPhysicsComponent> mapPhysics) {
    this->_map_helper = mapHelper;
    this->_map_physics = mapPhysics;

    mapGenerator->generate(w, h, this->map);
}

void GameMap::updateLogic(GameWorld* game_world) {
    if (_map_physics) {
        _map_physics->updateLogic(game_world);
    }
}

vector<float> perlinNoise2D(int width, int height, int octaves, float bias,
                            vector<float>& noiseSeed) {
    vector<float> output(width * height);

    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++) {
            float fNoise = 0.0f;
            float fScaleAcc = 0.0f;
            float fScale = 1.0f;

            for (int o = 0; o < octaves; o++) {
                int nPitch = width >> o;
                int nSampleX1 = (x / nPitch) * nPitch;
                int nSampleY1 = (y / nPitch) * nPitch;

                int nSampleX2 = (nSampleX1 + nPitch) % width;
                int nSampleY2 = (nSampleY1 + nPitch) % width;

                float fBlendX = (float)(x - nSampleX1) / (float)nPitch;
                float fBlendY = (float)(y - nSampleY1) / (float)nPitch;

                float fSampleT =
                    (1.0f - fBlendX) *
                        noiseSeed[nSampleY1 * width + nSampleX1] +
                    fBlendX * noiseSeed[nSampleY1 * width + nSampleX2];
                float fSampleB =
                    (1.0f - fBlendX) *
                        noiseSeed[nSampleY2 * width + nSampleX1] +
                    fBlendX * noiseSeed[nSampleY2 * width + nSampleX2];

                fScaleAcc += fScale;
                fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;
                fScale = fScale / bias;
            }

            output[y * width + x] = fNoise / fScaleAcc;
        }
    return move(output);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

const int MAXN = 500 + 7;

// 清除不联通区域的算法
struct MapAlg1 {
    int w, h;
    int color[MAXN][MAXN];
    bool vis[MAXN][MAXN];

    const int dx[4] = {0, 0, 1, -1};
    const int dy[4] = {1, -1, 0, 0};

    const int dx1[8] = {0, 0, 1, -1, 1, -1, 1, -1};
    const int dy1[8] = {1, -1, 0, 0, -1, 1, 1, -1};

    static const int B = 0;
    static const int G = 1000000;

    struct coor {
        int x, y;
    };

    bool inside(int x, int y) {
        if (x >= 1 && x <= w && y >= 1 && y <= h) {
            return true;
        }
        return false;
    }

    // 扩展
    void expand(int x, int y) {
        // 边缘块
        vector<coor> edge;
        // 这一颜色的所有块
        vector<coor> all;
        // 寻找所有边缘块，顺便处理出所有相同颜色的块
        search_edge({x, y}, edge, all);

        auto result = expand_edge(edge);

        auto start = result.first;
        auto end = result.second;

        float xx = end.x - start.x;
        float yy = end.y - start.y;
        float d = sqrt(xx * xx + yy * yy);

        float cos_th = xx / d;
        float sin_th = yy / d;

        int col = color[end.x][end.y];

        for (float d1 = 0.5f; d1 <= d; d1 += 0.5f) {
            int nx = d1 * cos_th;
            int ny = d1 * sin_th;
            occupy({start.x + nx, start.y + ny}, 1.5, col);
        }

        for (auto& it : all) {
            color[it.x][it.y] = col;
        }
    }
    // 占领这个块附近的块
    void occupy(coor cor, float r, int col) {
        for (int nx = cor.x - r; nx <= cor.x + r; ++nx) {
            for (int ny = cor.y - r; ny <= cor.y + r; ++ny) {
                if (!inside(nx, ny)) {
                    continue;
                }
                float xx = nx - cor.x;
                float yy = ny - cor.y;
                float d = sqrt(xx * xx + yy * yy);
                if (d <= r) {
                    color[nx][ny] = col;
                }
            }
        }
    }

    void search_edge(coor c, vector<coor>& edge /*边缘块的坐标*/,
                     vector<coor>& all) {
        memset(vis, false, sizeof(vis));

        queue<coor> que;
        que.push(c);
        while (!que.empty()) {
            auto cor = que.front();
            que.pop();
            if (vis[cor.x][cor.y]) {
                continue;
            }
            vis[cor.x][cor.y] = true;
            // 被推进队列的一定是相同颜色的
            all.push_back(cor);

            // cor这个格子是否是边界
            bool is_edge = false;
            // 遍历上下左右
            for (int k = 0; k < 4; ++k) {
                int nx = cor.x + dx[k];
                int ny = cor.y + dy[k];

                if (!inside(nx, ny)) {
                    continue;
                }
                // 挨着墙了
                if (color[nx][ny] == B) {
                    is_edge = true;
                }
                // 同样颜色的，拓展出去
                if (color[nx][ny] == color[cor.x][cor.y]) {
                    que.push({nx, ny});
                }
            }
            if (is_edge) {
                edge.push_back(cor);
            }
        }
    }

    pair<coor /*挖洞起始块*/, coor /*挖洞终点块*/> expand_edge(
        vector<coor>& edge) {
        memset(vis, false, sizeof(vis));

        coor result_start;
        coor result_end;

        const int start_color = color[edge[0].x][edge[0].y];

        queue<pair<coor /*拓展的开始块*/, coor /*当前坐标*/>> que;
        for (auto& it : edge) {
            que.push({it, it});
        }
        // 开始块一定为区域的颜色

        while (!que.empty()) {
            auto p = que.front();
            que.pop();

            auto& start = p.first;
            auto& now_cor = p.second;

            if (vis[now_cor.x][now_cor.y]) {
                continue;
            }
            vis[now_cor.x][now_cor.y] = true;

            // 不是墙，且是一种不同的颜色，可以拓展！
            if (color[now_cor.x][now_cor.y] != B &&
                color[now_cor.x][now_cor.y] != start_color) {
                result_start = start;
                result_end = now_cor;
                break;
            }

            for (int k = 0; k < 8; ++k) {
                int nx = now_cor.x + dx1[k];
                int ny = now_cor.y + dy1[k];
                if (!inside(nx, ny)) {
                    continue;
                }
                // 是自己颜色的块，不拓展
                if (color[nx][ny] == start_color) {
                    continue;
                }
                que.push({start, {nx, ny}});
            }
        }

        return {result_start, result_end};
    }

    void solve() {
        for (int i = 1; i <= w; ++i) {
            for (int j = 1; j <= h; ++j) {
                if (color[i][j] != B && color[i][j] != G) {
                    expand(i, j);
                }
            }
        }
    }
};

// bfs染色，并且使最大的空气区域成为特定值
struct MapAlg2 {
    int w, h;
    // 墙0，空气1
    int color[MAXN][MAXN];
    int color_size[MAXN * MAXN];
    bool vis[MAXN][MAXN];

    const int dx[4] = {0, 0, 1, -1};
    const int dy[4] = {1, -1, 0, 0};

    struct coor {
        int x, y;
    };

    bool inside(int x, int y) {
        if (x >= 1 && x <= w && y >= 1 && y <= h) {
            return true;
        }
        return false;
    }

    void bfs(const coor cor, int c) {
        if (vis[cor.x][cor.y]) {
            return;
        }

        queue<coor> que;
        que.push(cor);
        while (!que.empty()) {
            auto cor = que.front();
            que.pop();

            if (vis[cor.x][cor.y]) {
                continue;
            }
            vis[cor.x][cor.y] = true;

            color[cor.x][cor.y] = c;
            color_size[c] += 1;

            for (int k = 0; k < 4; ++k) {
                int nx = cor.x + dx[k];
                int ny = cor.y + dy[k];
                if (!inside(nx, ny)) {
                    continue;
                }
                if (color[nx][ny] != 1) {
                    continue;
                }

                que.push({nx, ny});
            }
        }
    }

    void start() {
        memset(vis, 0, sizeof(vis));

        int cc = 2;
        for (int i = 1; i <= w; ++i) {
            for (int j = 1; j <= h; ++j) {
                if (color[i][j] == 1) {
                    bfs({i, j}, cc);
                    cc += 1;
                }
            }
        }

        int mmax = 0;
        int mmax_c = 2;
        for (int i = 2; i < cc; ++i) {
            if (color_size[i] > mmax) {
                mmax = color_size[i];
                mmax_c = i;
            }
        }

        for (int i = 1; i <= w; ++i) {
            for (int j = 1; j <= h; ++j) {
                if (color[i][j] == mmax_c) {
                    color[i][j] = 1000000;
                }
            }
        }
    }
};

void MapGeneratorComponent1::init(unsigned int seed) {
    random = make_shared<Random>(seed);
}

void MapGeneratorComponent1::generate(int w, int h, MapTile& map) {
    const auto air = MapTileType::air;
    const auto dirt = MapTileType::dirt;
    const auto grass = MapTileType::grass;
    const auto rock = MapTileType::rock;

    // 参数
    const int tunc = 110;
    const int octaveCount = 7;
    const float scalingBias = 0.3f;

    vector<float> noiseSeed((w + 1) * (h + 1), 0.0f);

    rand_float r0(0.0f, 1.0f);
    for (auto& it : noiseSeed) {
        it = r0(*random);
    }

    vector<float> noise2D =
        move(perlinNoise2D(w, h, octaveCount, scalingBias, noiseSeed));

    for (int i = 1; i <= w; ++i) {
        for (int j = 1; j <= h; ++j) {
            map[i][j] = dirt;
        }
    }

    for (int x = 1; x <= w; x++) {
        for (int y = 1; y <= h; y++) {
            float k = noise2D[(y - 1) * w + (x - 1)];
            int a = 255.0f * k;
            if (a > tunc) {
                map[x][y] = air;
            }
        }
    }

    // 边界两格泥土填充

    // 左纵
    for (int x = 1; x <= 2; ++x) {
        for (int y = 1; y <= h; ++y) {
            map[x][y] = dirt;
        }
    }

    // 右纵
    for (int x = w - 1; x <= w; ++x) {
        for (int y = 1; y <= h; ++y) {
            map[x][y] = dirt;
        }
    }

    // 横下
    for (int y = 1; y <= 2; ++y) {
        for (int x = 1; x <= w; ++x) {
            map[x][y] = dirt;
        }
    }

    // 横上
    for (int y = h - 1; y <= h; ++y) {
        for (int x = 1; x <= w; ++x) {
            map[x][y] = dirt;
        }
    }

    auto alg1 = make_shared<MapAlg1>();
    auto alg2 = make_shared<MapAlg2>();

    // bfs染色
    alg2->w = w;
    alg2->h = h;
    for (int x = 1; x <= w; x++) {
        for (int y = 1; y <= h; y++) {
            if (map[x][y] == dirt) {
                alg2->color[x][y] = 0;
            }
            if (map[x][y] == air) {
                alg2->color[x][y] = 1;
            }
        }
    }
    alg2->start();

    for (int x = 1; x <= w; x++) {
        for (int y = 1; y <= h; y++) {
            alg1->color[x][y] = alg2->color[x][y];
        }
    }
    alg1->w = w;
    alg1->h = h;
    alg1->solve();

    for (int x = 1; x <= w; x++) {
        for (int y = 1; y <= h; y++) {
            if (alg1->color[x][y] == 1000000) {
                map[x][y] = air;
            }
            if (alg1->color[x][y] == 0) {
                map[x][y] = dirt;
            }
        }
    }
    ///////////////////////////////////////////////////////////////////

    // 随机岩石
    rand_int r3(0, 3);
    for (int x = 1; x <= w; x++) {
        for (int y = 1; y <= h; y++) {
            if (map[x][y] == dirt) {
                int temp = 0;
                // 随机与边缘泥土块的距离
                int dis = r3(*random);

                if (y - dis >= 0) {
                    if (map[x][y - dis] != air) {
                        ++temp;
                    }
                }

                if (y + dis <= h) {
                    if (map[x][y + dis] != air) {
                        ++temp;
                    }
                }

                if (x - dis >= 1) {
                    if (map[x - dis][y] != air) {
                        ++temp;
                    }
                }

                if (x + dis <= w) {
                    if (map[x + dis][y] != air) {
                        ++temp;
                    }
                }

                if (temp == 4) {
                    map[x][y] = rock;
                }
            }
        }
    }

    // 草地
    for (int x = 1; x <= w; x++) {
        for (int y = 1; y <= h; y++) {
            if (map[x][y] == dirt) {
                if (y + 1 <= h) {
                    if (map[x][y + 1] == air) {
                        map[x][y] = grass;
                    }
                }
            }
        }
    }

    // 测试区域掏空
    for (int x = 3; x <= 6; ++x) {
        for (int y = 3; y <= 6; ++y) {
            map[x][y] = air;
        }
    }

    // HASH 验证
    const auto hash = [&](int key) {
        const int P = 1e9 + 7;
        return key % P;
    };

    int k = 1;
    for (int i = 1; i <= w; ++i) {
        for (int j = 1; j <= h; ++j) {
            k = hash((int)map[i][j] * 13 + k);
        }
    }
    CCLOG("GameMap HASH: %d", k);
}

void MapGeneratorComponent2::init(unsigned seed) {}

void MapGeneratorComponent2::generate(int w, int h, MapTile& map) {
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

///////////////////////////////////////////////////////////////////////////////

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

void MapPhysicsComponent1::updateLogic(GameWorld* game_world) {
    const int dx[8] = {-1, 0, 1, 1, 1, 0, -1, -1};
    const int dy[8] = {1, 1, 1, 0, -1, -1, -1, 0};

    auto _map = game_world->getGameMap();

    const auto inside = [&](int x, int y) {
        if (x >= 1 && x <= _map->get().w && y >= 1 && y <= _map->get().h) {
            return true;
        }
        return false;
    };

    const auto make_uid = [&](const iVec2& pos) {
        stringstream ss;
        if (pos.x < 10) {
            ss << "00" << pos.x;
        } else if (pos.x < 100) {
            ss << "0" << pos.x;
        } else {
            ss << pos.x;
        }
        if (pos.y < 10) {
            ss << "00" << pos.y;
        } else if (pos.y < 100) {
            ss << "0" << pos.y;
        } else {
            ss << pos.y;
        }
        return string(ss.str());
    };

    auto& quad = game_world->get_objects();
    quad.visit_in_rect(
        {-1, 500}, {500, -1}, [&](const iVec2& cor, GameObject* ob) {
            auto type = ob->getGameObjectType();
            if (type != object_type_player && type != object_type_bullet &&
                type != object_type_enemy && type != object_type_equipment) {
                return;
            }

            auto box = ob->getBoundingBox();
            auto left_bottom = _map->_map_helper->convert_in_map(box.origin);
            auto right_top =
                _map->_map_helper->convert_in_map(box.origin + box.size);

            for (int xx = left_bottom.x; xx <= right_top.x; ++xx) {
                for (int yy = left_bottom.y; yy <= right_top.y; ++yy) {
                    auto map_type = _map->get()[xx][yy];
                    if (map_type == air) {
                        continue;
                    }
                    //
                    auto uid = make_uid({xx, yy});
                    auto iter = _dirty.find(uid);
                    if (iter != _dirty.end()) {
                        json event;
                        event["type"] = "refresh";
                        iter->second->pushEvent(event);
                    } else {
                        auto sp = game_world->newObject(
                            4, Vec2((xx - 1) * 64 + 5,
                                    (yy - 1) * 64 + 5) /*保证落在格子里*/);
                        sp->initWithSpriteFrameName(
                            "game_map_tile_physics.png");
                        sp->setVisible(false);

                        sp->setGameObjectType(object_type_wall);
                        sp->setAnchorPoint(Vec2(0, 0));
                        sp->setPosition(Vec2((xx - 1) * 64, (yy - 1) * 64));

                        auto comp = make_shared<MapGameObjectTileComponent>();
                        comp->init(&_dirty, uid);
                        sp->addGameComponent(comp);

                        PhysicsShapeCache::getInstance()->setBodyOnSprite(
                            "game_map_tile_physics", sp);

                        _dirty.insert({uid, sp});
                    }
                }
            }
        });
}

void MapGameObjectTileComponent::updateLogic(GameObject* ob) {
    cnt -= 1;
    if (cnt == 0) {
        _dirty->erase(uid);
        ob->removeFromParent();
    }
}

void MapGameObjectTileComponent::receiveEvent(GameObject* ob,
                                              const json& event) {
    string type = event["type"];
    if (type == "refresh") {
        cnt = 10;
    }
}
