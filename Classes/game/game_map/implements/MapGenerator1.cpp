#include "MapGenerator1.h"

const int MAXN = 500 + 7;

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

// �������ͨ������㷨
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

    // ��չ
    void expand(int x, int y) {
        // ��Ե��
        vector<coor> edge;
        // ��һ��ɫ�����п�
        vector<coor> all;
        // Ѱ�����б�Ե�飬˳�㴦���������ͬ��ɫ�Ŀ�
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
    // ռ������鸽���Ŀ�
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

    void search_edge(coor c, vector<coor>& edge /*��Ե�������*/,
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
            // ���ƽ����е�һ������ͬ��ɫ��
            all.push_back(cor);

            // cor��������Ƿ��Ǳ߽�
            bool is_edge = false;
            // ������������
            for (int k = 0; k < 4; ++k) {
                int nx = cor.x + dx[k];
                int ny = cor.y + dy[k];

                if (!inside(nx, ny)) {
                    continue;
                }
                // ����ǽ��
                if (color[nx][ny] == B) {
                    is_edge = true;
                }
                // ͬ����ɫ�ģ���չ��ȥ
                if (color[nx][ny] == color[cor.x][cor.y]) {
                    que.push({nx, ny});
                }
            }
            if (is_edge) {
                edge.push_back(cor);
            }
        }
    }

    pair<coor /*�ڶ���ʼ��*/, coor /*�ڶ��յ��*/> expand_edge(
        vector<coor>& edge) {
        memset(vis, false, sizeof(vis));

        coor result_start;
        coor result_end;

        const int start_color = color[edge[0].x][edge[0].y];

        queue<pair<coor /*��չ�Ŀ�ʼ��*/, coor /*��ǰ����*/>> que;
        for (auto& it : edge) {
            que.push({it, it});
        }
        // ��ʼ��һ��Ϊ�������ɫ

        while (!que.empty()) {
            auto p = que.front();
            que.pop();

            auto& start = p.first;
            auto& now_cor = p.second;

            if (vis[now_cor.x][now_cor.y]) {
                continue;
            }
            vis[now_cor.x][now_cor.y] = true;

            // ����ǽ������һ�ֲ�ͬ����ɫ��������չ��
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
                // ���Լ���ɫ�Ŀ飬����չ
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

// bfsȾɫ������ʹ���Ŀ��������Ϊ�ض�ֵ
struct MapAlg2 {
    int w, h;
    // ǽ0������1
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

void MapGenerator1::init(unsigned int seed) {
    random = make_shared<Random>(seed);
}

void MapGenerator1::generate(int w, int h, MapTile& map) {
    const auto air = MapTileType::air;
    const auto dirt = MapTileType::dirt;
    const auto grass = MapTileType::grass;
    const auto rock = MapTileType::rock;

    // ����
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

    // �߽������������

    // ����
    for (int x = 1; x <= 2; ++x) {
        for (int y = 1; y <= h; ++y) {
            map[x][y] = dirt;
        }
    }

    // ����
    for (int x = w - 1; x <= w; ++x) {
        for (int y = 1; y <= h; ++y) {
            map[x][y] = dirt;
        }
    }

    // ����
    for (int y = 1; y <= 2; ++y) {
        for (int x = 1; x <= w; ++x) {
            map[x][y] = dirt;
        }
    }

    // ����
    for (int y = h - 1; y <= h; ++y) {
        for (int x = 1; x <= w; ++x) {
            map[x][y] = dirt;
        }
    }

    auto alg1 = make_shared<MapAlg1>();
    auto alg2 = make_shared<MapAlg2>();

    // bfsȾɫ
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

    // �����ʯ
    rand_int r3(0, 3);
    for (int x = 1; x <= w; x++) {
        for (int y = 1; y <= h; y++) {
            if (map[x][y] == dirt) {
                int temp = 0;
                // ������Ե������ľ���
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

    // �ݵ�
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

    // ���������Ϳ�
    for (int x = 3; x <= 8; ++x) {
        for (int y = h - 7; y <= h - 2; ++y) {
            map[x][y] = air;
        }
    }

    // ����ƽ̨
    for (int x = 3; x <= 5; ++x) {
        int y = h - 8;
        map[x][y] = rock;
    }

    // HASH ��֤
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
