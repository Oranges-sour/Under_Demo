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
    this->_globalRandom = make_shared<Random>(31415);

    this->schedule([&](float) { main_update_logic(); }, 0.05f, "update_logic");
    this->schedule([&](float) { main_update_draw(); }, "update_draw");

    this->_game_node = Node::create();
    this->addChild(_game_node, 0);

    this->game_bk_target = Node::create();
    game_bk_target->setAnchorPoint(Vec2(0, 0));
    game_bk_target->setPosition(0, 0);
    this->addChild(game_bk_target, -2);

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

void GameWorld::pushGameAct(const GameAct& act) { _game_act_que.push(act); }

void GameWorld::cleanup() {
    if (_gameRenderer) {
        _gameRenderer->release();
    }
    Node::cleanup();
}

GameObject* GameWorld::newObject(ObjectLayer layer, const Vec2& startPos) {
    auto ob = GameObject::create();
    assert(ob != nullptr);

    ob->init(this);
    ob->setPosition(startPos);
    _game_node->addChild(ob, layer);

    auto p = this->_gameMap->getMapHelper()->convert_in_map(startPos);

    ob->setUID(Tools::random_string(8, *_globalRandom));

    needToAdd.insert({ob, p});

    return ob;
}

void GameWorld::removeObject(GameObject* ob) { needToRemove.insert(ob); }

void GameWorld::main_update_logic() {
    while (!_game_act_que.empty()) {
        auto p = _game_act_que.front();
        _game_act_que.pop();

        auto iter = _game_objects.find(p.uid);
        if (iter != _game_objects.end()) {
            iter->second->pushGameAct(p);
        }
    }

    quad_tree.visit_in_rect(
        {-1, 500}, {500, -1},
        [&](const iVec2& cor, GameObject* ob) { ob->main_update(); });

    this->updateGameObjectPosition();

    _gameMap->updateLogic(this);

    // 添加
    for (auto& it : needToAdd) {
        _game_objects.insert({it.first->getUID(), it.first});

        auto quad_node = quad_tree.insert(it.second, it.first);
        it.first->quad_node = quad_node;
    }
    needToAdd.clear();

    // 移除
    for (auto& it : needToRemove) {
        if (it == camera_follow_object) {
            camera_follow_object = nullptr;
        }

        it->quad_node.container->remove(it->quad_node.uid);

        _game_objects.erase(it->getUID());

        it->Sprite::removeFromParent();
        // SpritePool::saveBack(it);
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

    game_bk_target->setPosition((-camera_pos + screenCenter) / 5);

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
            if (type != object_type_player && type != object_type_bullet &&
                type != object_type_enemy && type != object_type_equipment) {
                return;
            }

            auto p = this->getGameMap()->getMapHelper()->convert_in_map(
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