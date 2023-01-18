#ifndef __MAP_PRERENDERER_1_H__
#define __MAP_PRERENDERER_1_H__

#include "game/game_map/GameMap.h"

class MapPreRendererComponent1 : public MapPreRendererComponent {
private:
    struct MapArea {
        int x, y, w, h;
    };

public:
    void init(MapTile* map, unsigned int seed);

    virtual bool isPreRenderFinish();

    virtual void preRender();

    // 返回整个地图的大小
    virtual Size afterPreRender(Node* target);

private:
    virtual void init(MapTile* map) override {}

    Texture2D* render(const MapArea& area);

    void createTile(MapTileType type, int bit_mask, const Vec2& pos,
                    vector<Node*>& dst);

    void createDecoration(int x, int y, MapTileType type, int bit_mask,
                          const Vec2& pos, const MapArea& area,
                          vector<Node*>& dst);

    void createTileMask(MapTileType type, int bit_mask, const Vec2& pos,
                        vector<Node*>& dst);

private:
    void createDec1(int x, int y, MapTileType type, int bit_mask,
                    const Vec2& pos, const MapArea& area, vector<Node*>& dst);
    void createDec2(int x, int y, MapTileType type, int bit_mask,
                    const Vec2& pos, const MapArea& area, vector<Node*>& dst);
    void createDec3(int x, int y, MapTileType type, int bit_mask,
                    const Vec2& pos, const MapArea& area, vector<Node*>& dst);
    void createDec4(int x, int y, MapTileType type, int bit_mask,
                    const Vec2& pos, const MapArea& area, vector<Node*>& dst);

    void createDec5(int x, int y, MapTileType type, int bit_mask,
                    const Vec2& pos, const MapArea& area, vector<Node*>& dst);

    void createDec6(int x, int y, MapTileType type, int bit_mask,
                    const Vec2& pos, const MapArea& area, vector<Node*>& dst);

private:
    shared_ptr<Random> _random;

    vector<bool> mark1;
    vector<bool> mark;

    MapTile* _map = nullptr;
    bool _isPreRenderFinish = false;

    int x_now, x_max, y_now, y_max;
    vector<pair<MapArea, Texture2D*>> textures;
};

#endif