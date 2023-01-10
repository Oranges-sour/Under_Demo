#include "GameWorld.h"

#include "GameMap.h"
#include "GameObject.h"
#include "MyMath.h"
#include "SpritePool.h"

GameWorld* GameWorld::create() {
    auto g = new GameWorld();
    if (g && g->init()) {
        g->autorelease();
        return g;
    }
    return nullptr;
}

bool GameWorld::init() {
    SpritePool::init(1200);
    this->schedule([&](float) { main_update_logic(); }, 0.05f, "update_logic");
    this->schedule([&](float) { main_update_draw(); }, "update_draw");

    this->_game_node = Node::create();
    this->addChild(_game_node, 0);

    this->game_map_target = Node::create();
    game_map_target->setAnchorPoint(Vec2(0, 0));
    game_map_target->setPosition(0, 0);
    _game_node->addChild(game_map_target, -1);

    this->game_renderer_target = Node::create();
    game_renderer_target->setAnchorPoint(Vec2(0, 0));
    game_renderer_target->setPosition(0, 0);
    this->addChild(game_renderer_target, 1);

    auto conatctListener = EventListenerPhysicsContact::create();

    conatctListener->onContactBegin = [&](PhysicsContact& conatct) -> bool {
        processContact(conatct);
        return false;
    };
    conatctListener->onContactPreSolve =
        [](PhysicsContact&, PhysicsContactPreSolve&) -> bool { return false; };
    conatctListener->onContactPostSolve =
        [](PhysicsContact&, const PhysicsContactPostSolve&) -> void {};
    conatctListener->onContactSeparate = [](PhysicsContact&) -> void {};

    Director::getInstance()
        ->getEventDispatcher()
        ->addEventListenerWithSceneGraphPriority(conatctListener, this);
    return true;
}

GameObject* GameWorld::newObject(int layer, const Vec2& startPos) {
    auto ob = SpritePool::getSprite();
    assert(ob != nullptr);

    ob->init(this);
    ob->setPosition(startPos);
    _game_node->addChild(ob, layer);

    auto p = this->_gameMap->_map_helper->convert_in_map(startPos);

    needToAdd.insert({ob, p});

    return ob;
}

void GameWorld::removeObject(GameObject* ob) { needToRemove.insert(ob); }

void GameWorld::main_update_logic() {
    quad_tree.visit_in_rect(
        {-1, 500}, {500, -1},
        [&](const iVec2& cor, GameObject* ob) { ob->main_update(); });

    this->updateGameObjectPosition();
    _gameMap->updateLogic(this);

    // ���
    for (auto& it : needToAdd) {
        auto quad_node = quad_tree.insert(it.second, it.first);
        it.first->quad_node = quad_node;
    }
    needToAdd.clear();

    // �Ƴ�
    for (auto& it : needToRemove) {
        if (it == camera_follow_object) {
            camera_follow_object = nullptr;
        }
        it->Sprite::removeFromParent();
        it->quad_node.container->remove(it->quad_node.uid);
        SpritePool::saveBack(it);
    }
    needToRemove.clear();
}

void GameWorld::main_update_draw() {
    if (camera_follow_object) {
        camera_pos_target = camera_follow_object->getPosition();
    }

    camera_pos +=
        _gameRenderer->calcu_camera_speed(camera_pos, camera_pos_target);

    auto screenCenter = Vec2(1920, 1080) / 2;

    _game_node->setPosition(-camera_pos + screenCenter);

    _gameRenderer->update(camera_pos - screenCenter, Size(1920, 1080), this);
}

void GameWorld::processContact(PhysicsContact& conatct) {
    auto const shapeA = conatct.getShapeA();
    auto const shapeB = conatct.getShapeB();

    auto objectA = dynamic_cast<GameObject*>(shapeA->getBody()->getNode());
    auto objectB = dynamic_cast<GameObject*>(shapeB->getBody()->getNode());

    if (!objectA || !objectB) {
        return;
    }

    {
        json event;
        event["type"] = "contact";
        event["object"] = (long long)objectB;
        objectA->pushEvent(event);
    }

    {
        json event;
        event["type"] = "contact";
        event["object"] = (long long)objectA;
        objectB->pushEvent(event);
    }
}

void GameWorld::updateGameObjectPosition() {
    vector<pair<GameObject*, iVec2>> need_to_update;

    quad_tree.visit_in_rect(
        {-1, 500}, {500, -1}, [&](const iVec2& cor, GameObject* ob) {
            auto type = ob->getGameObjectType();
            if (type != player && type != bullet && type != enemy &&
                type != equipment) {
                return;
            }

            auto p = this->getGameMap()->_map_helper->convert_in_map(
                ob->getPosition());
            if (cor.x != p.x || cor.y != p.x) {
                need_to_update.push_back({ob, {p.x, p.y}});
            }
        });

    for (auto& it : need_to_update) {
        auto& ob = it.first;
        auto& node = ob->quad_node;
        node.container->remove(node.uid);

        node = quad_tree.insert({it.second}, ob);
    }
}

///////////////////////////////////////////////////////////////////////////////////////

GameWorldRenderer1::~GameWorldRenderer1() {
    if (render) {
        render->release();
    }
    for (auto& it : lights) {
        it->release();
    }
}

void GameWorldRenderer1::init(Node* target) {
    this->light = new Sprite();
    light->setAnchorPoint(Vec2(0, 0));
    light->setPosition(1920, 0);
    light->setScaleX(-1);
    target->addChild(light);

    this->render = RenderTexture::create(1920, 1080);
    this->render->retain();

    lights.resize(100);
    for (auto& it : lights) {
        it = Sprite::createWithSpriteFrameName("light.png");
        it->retain();
    }
}

void GameWorldRenderer1::update(const Vec2& left_bottom, const Size& size,
                                GameWorld* gameworld) {
    auto gameMap = gameworld->getGameMap();
    auto pp = gameMap->_map_helper->convert_in_map(left_bottom);
    int xx = pp.x;
    int yy = pp.y;

    auto pp1 =
        gameMap->_map_helper->convert_in_map(Vec2(size.width, size.height));
    int ww = pp1.x;
    int hh = pp1.y;

    auto& quad = gameworld->get_objects();

    int cnt = 0;

    quad.visit_in_rect({xx, yy + hh}, {xx + ww, yy},
                       [&](const iVec2& coor, GameObject* object) {
                           if (cnt == 100) {
                               return;
                           }
                           auto p = object->getPosition();
                           auto r = object->getLightRadius();
                           auto c = object->getLightColor();

                           lights[cnt]->setPosition(p - left_bottom);
                           lights[cnt]->setScale(r / 100.0f);
                           lights[cnt]->setColor(c);

                           cnt += 1;
                       });
    // 0.2 �����ñ�������ȫ��

    render->beginWithClear(0.2, 0.2, 0.2, 1);
    for (int i = 0; i < cnt; ++i) {
        lights[i]->visit();
    }
    render->end();

    light->setSpriteFrame(render->getSprite()->getSpriteFrame());
    light->setBlendFunc({GL_DST_COLOR, GL_ZERO});
}

Vec2 GameWorldRenderer1::calcu_camera_speed(const Vec2& current_pos,
                                            const Vec2& target_pos) {
    // �������������
    const auto calcuSpeed = [](float x) -> float {
        return (x * x) / (12 * (x + 3));
    };
    auto del = target_pos - current_pos;
    float dis = del.distance(Vec2(0, 0));
    float speed = calcuSpeed(dis);
    auto move_pos = MyMath::getPosOnLine(Vec2(0, 0), del, speed);

    return move_pos;
}
