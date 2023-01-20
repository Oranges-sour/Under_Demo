#ifndef __QUAD_TREE_H__
#define __QUAD_TREE_H__

#include <functional>
#include <memory>
#include <set>
#include <string>

#include "utility/math/Random.h"
#include "utility/Tools.h"
#include "utility/math/iVec2.h"

template <class T>
class QuadContainer {
public:
    std::string store(const T &val) {
        std::string key = Tools::random_string(8);

        contain.insert({key, val});

        return key;
    }

    void remove(const std::string &uid) { contain.erase(uid); }

public:
    std::map<std::string, T> contain;
};

template <class T>
struct Quad_node {
    Quad_node() { uid = "bad_uid"; }
    Quad_node(std::shared_ptr<QuadContainer<T>> container,
              const std::string &uid, const iVec2 &coor)
        : container(container), uid(uid), coor(coor) {}

    std::shared_ptr<QuadContainer<T>> container;
    std::string uid;
    iVec2 coor;
};

template <class T>
class Quad {
public:
    Quad(const iVec2 &left_top, const iVec2 &right_bottom) : _child(false) {
        this->left_top = left_top;
        this->right_bottom = right_bottom;

        // 最后一层,要存节点
        if (abs(left_top.x - right_bottom.x) < 1 &&
            abs(left_top.y - right_bottom.y) < 1) {
            container = std::make_shared<QuadContainer<T>>();

            _child = true;
        }
    }

    Quad_node<T> insert(const iVec2 &_where, const T &val) {
        if (!inside(_where)) {
            return Quad_node<T>{};
        }

        if (_child) {
            auto uid = container->store(val);
            return Quad_node<T>{container, uid, _where};
        }

        int idx = get_and_new_quads(_where);

        return quads[idx]->insert(_where, val);
    }

    void visit_in_rect(
        const iVec2 &left_top, const iVec2 &right_bottom,
        const std::function<void(const iVec2 &cor, T &val)> &func) const {
        // 判断界外
        if (this->right_bottom.x < left_top.x ||
            this->left_top.x > right_bottom.x ||
            this->right_bottom.y > left_top.y ||
            this->left_top.y < right_bottom.y) {
            return;
        }

        if (_child) {
            for (auto &it : container->contain) {
                func(left_top, it.second);
            }
        } else {
            for (int i = 1; i <= 4; ++i) {
                if (quads[i].get()) {
                    quads[i]->visit_in_rect(left_top, right_bottom, func);
                }
            }
        }
    }

    bool inside(const iVec2 &cor) {
        if (cor.x >= left_top.x && cor.x <= right_bottom.x &&
            cor.y >= right_bottom.y && cor.y <= left_top.y) {
            return true;
        }
        return false;
    }

private:
    int get_and_new_quads(const iVec2 &_where) {
        int x_mid = (left_top.x + right_bottom.x) >> 1;
        int y_mid = (left_top.y + right_bottom.y) >> 1;

        int pp = 0;
        // 判断象限
        if (_where.x <= x_mid) {
            if (_where.y <= y_mid) {
                pp = 3;
            } else {
                pp = 2;
            }
        } else {
            if (_where.y <= y_mid) {
                pp = 4;
            } else {
                pp = 1;
            }
        }

        // 还没创建过要创建
        if (quads[pp].get() == nullptr) {
            if (pp == 1) {
                quads[1] =
                    std::make_shared<Quad<T>>(iVec2{x_mid + 1, left_top.y},
                                              iVec2{right_bottom.x, y_mid + 1});
            } else if (pp == 2) {
                quads[2] = std::make_shared<Quad<T>>(
                    iVec2{left_top.x, left_top.y}, iVec2{x_mid, y_mid + 1});
            } else if (pp == 3) {
                quads[3] = std::make_shared<Quad<T>>(
                    iVec2{left_top.x, y_mid}, iVec2{x_mid, right_bottom.y});
            } else if (pp == 4) {
                quads[4] = std::make_shared<Quad<T>>(
                    iVec2{x_mid + 1, y_mid},
                    iVec2{right_bottom.x, right_bottom.y});
            }
        }

        return pp;
    }
    iVec2 left_top;
    iVec2 right_bottom;

    bool _child;

    // 最后一层,存元素的容器
    std::shared_ptr<QuadContainer<T>> container;

    // 四个子象限 1,2,3,4. 0不用
    std::shared_ptr<Quad<T>> quads[5];
};

#endif