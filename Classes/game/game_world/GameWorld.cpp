#include "GameWorld.h"

#include "game/game_map/GameMap.h"
#include "game/game_object/GameObject.h"

GameWorld* GameWorld::create() {
    auto g = new GameWorld();
    if (g && g->init()) {
        g->autorelease();
        return g;
    }
    return nullptr;
}

bool GameWorld::init() {
    // 初始化全局随机数
    this->_global_random = make_shared<Random>(31415);

    this->schedule([&](float) { mainUpdateLogic(); }, 0.05f, "update_logic");
    this->schedule([&](float) { mainUpdateDraw(); }, "update_draw");

    this->_game_node = Node::create();
    this->addChild(_game_node, 0);

    this->_game_bk_target = Node::create();
    _game_bk_target->setAnchorPoint(Vec2(0, 0));
    _game_bk_target->setPosition(0, 0);
    this->addChild(_game_bk_target, -2);

    this->_game_map_target = Node::create();
    _game_map_target->setAnchorPoint(Vec2(0, 0));
    _game_map_target->setPosition(0, 0);
    _game_node->addChild(_game_map_target, -1);

    this->_game_renderer_target = Node::create();
    _game_renderer_target->setAnchorPoint(Vec2(0, 0));
    _game_renderer_target->setPosition(0, 0);
    this->addChild(_game_renderer_target, 1);

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

void GameWorld::pushGameAct(const GameAct& act) { _game_act_que.push(act); }

void GameWorld::cleanup() {
    if (_game_renderer) {
        _game_renderer->release();
    }
    Node::cleanup();
}

GameObject* GameWorld::newObject(ObjectLayer layer, const Vec2& startPos) {
    auto ob = GameObject::create();
    assert(ob != nullptr);

    ob->init(this);
    ob->setPosition(startPos);
    _game_node->addChild(ob, layer);

    auto p = this->_game_map->getMapHelper()->convertInMap(startPos);

    ob->setUID(Tools::randomString(8, *_global_random));

    _need_to_add.insert({ob, p});

    return ob;
}

void GameWorld::removeObject(GameObject* ob) { _need_to_remove.insert(ob); }

void GameWorld::mainUpdateLogic() {
    while (!_game_act_que.empty()) {
        auto p = _game_act_que.front();
        _game_act_que.pop();

        auto iter = _game_objects.find(p.uid);
        if (iter != _game_objects.end()) {
            iter->second->pushGameAct(p);
        }
    }

    CCLOG("--frame--");

    // 全图更新
    _quad_tree.visitInRect(
        {1, _game_map->get()._h}, {_game_map->get()._w, 1},
        [&](const iVec2& cor, GameObject* ob) { ob->mainUpdate(); });

    const auto visibleSize = Director::getInstance()->getVisibleSize();

    // 摄像机区域更新
    const auto screenCenter = Vec2(visibleSize.width, visibleSize.height) / 2;
    mainUpdateInScreenRect(_camera_pos - screenCenter, visibleSize);

    this->updateGameObjectPosition();

    _game_map->updateLogic(this);

    // 添加
    for (auto& it : _need_to_add) {
        _game_objects.insert({it.first->getUID(), it.first});

        auto quad_node = _quad_tree.insert(it.second, it.first);
        it.first->_quad_node = quad_node;
    }
    _need_to_add.clear();

    // 移除
    for (auto& it : _need_to_remove) {
        if (it == _camera_follow_object) {
            _camera_follow_object = nullptr;
        }

        it->_quad_node._container->remove(it->_quad_node._uid);

        _game_objects.erase(it->getUID());

        it->Sprite::removeFromParent();
        // SpritePool::saveBack(it);
    }
    _need_to_remove.clear();
}

void GameWorld::mainUpdateDraw() {
    if (_camera_follow_object) {
        _camera_pos_target = _camera_follow_object->getPosition();
    }

    _camera_pos +=
        _game_renderer->calcuCameraSpeed(_camera_pos, _camera_pos_target);

    const auto visibleSize = Director::getInstance()->getVisibleSize();

    auto screenCenter = Vec2(visibleSize.width, visibleSize.height) / 2;

    _game_node->setPosition(-_camera_pos + screenCenter);

    _game_bk_target->setPosition((-_camera_pos + screenCenter) / 5);

    _game_renderer->update(_camera_pos - screenCenter, visibleSize, this);
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

    _quad_tree.visitInRect(
        {-1, 500}, {500, -1}, [&](const iVec2& cor, GameObject* ob) {
            auto type = ob->getGameObjectType();
            if (type != object_type_player && type != object_type_bullet &&
                type != object_type_enemy && type != object_type_equipment) {
                return;
            }

            auto p = this->getGameMap()->getMapHelper()->convertInMap(
                ob->getPosition());
            if (cor.x != p.x || cor.y != p.x) {
                need_to_update.push_back({ob, {p.x, p.y}});
            }
        });

    for (auto& it : need_to_update) {
        auto& ob = it.first;
        auto& node = ob->_quad_node;
        node._container->remove(node._uid);

        node = _quad_tree.insert({it.second}, ob);
    }
}

void GameWorld::mainUpdateInScreenRect(const Vec2& left_bottom,
                                           const Size& size) {
    auto ilb = _game_map->getMapHelper()->convertInMap(left_bottom);

    auto isize =
        _game_map->getMapHelper()->convertInMap(Vec2(size.width, size.height));

    // 边缘扩大一点
    _quad_tree.visitInRect({ilb.x - 5, ilb.y + isize.y + 5},
                            {ilb.x + isize.x + 5, ilb.y - 5},
                            [&](const iVec2& coor, GameObject* object) {
                                object->mainUpdateInScreenRect();
                            });
}
