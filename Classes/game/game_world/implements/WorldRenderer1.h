#ifndef __WORLD_RENDERER_1_H__
#define __WORLD_RENDERER_1_H__

#include "game/game_world/GameWorld.h"

class GameWorldRenderer1 : public GameWorldRenderer {
public:
    virtual void init(Node* target) override;
    virtual void release() override;
    virtual void update(const Vec2& left_bottom, const Size& size,
                        GameWorld* gameworld) override;
    virtual Vec2 calcuCameraSpeed(const Vec2& current_pos,
                                    const Vec2& target_pos) override;

private:
    Sprite* light = nullptr;
    RenderTexture* render = nullptr;

    vector<Sprite*> lights[3];
};


#endif
