#include "SpritePool.h"

vector<GameObject*> SpritePool::pool = vector<GameObject*>{};
GameObject* SpritePool::first_available = nullptr;
int SpritePool::max_size = 0;

void SpritePool::init(int max_size) {
    if (pool.size() != 0) {
        for (auto& it : pool) {
            delete it;
        }
        pool.resize(0);
    }
    if (max_size == 0) {
        return;
    }
    SpritePool::max_size = max_size;
    pool.resize(max_size);
    for (int i = 0; i < max_size; ++i) {
        pool[i] = new GameObject();
    }

    first_available = pool[0];

    for (int i = 0; i < max_size - 1; ++i) {
        auto& no = pool[i];
        no->setNext(pool[i + 1]);
    }
    pool[max_size - 1]->setNext(nullptr);
}

GameObject* SpritePool::getSprite() {
    auto result = first_available;
    if (!result) {
        return nullptr;
    }
    first_available = result->getNext();

    result->setInUse(true);
    return result;
}

void SpritePool::saveBack(GameObject* game_object) {
    auto now = first_available;
    first_available = game_object;
    game_object->setNext(now);

    game_object->setInUse(false);
}
