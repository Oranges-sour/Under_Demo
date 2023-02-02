#ifndef __QUAD_TREE_H__
#define __QUAD_TREE_H__

#include <functional>
#include <memory>
#include <set>
#include <string>

#include "utility/Tools.h"
#include "utility/math/Random.h"
#include "utility/math/iVec2.h"

template <class T>
class QuadContainer {
public:
    std::string store(const T &val) {
        std::string key = Tools::randomString(8);

        _container.insert({key, val});

        return key;
    }

    void remove(const std::string &uid) { _container.erase(uid); }

public:
    std::map<std::string, T> _container;
};

template <class T>
struct QuadNode {
    QuadNode() { _uid = "bad_uid"; }
    QuadNode(std::shared_ptr<QuadContainer<T>> container,
             const std::string &uid, const iVec2 &coor)
        : _container(container), _uid(uid), _coor(coor) {}

    std::shared_ptr<QuadContainer<T>> _container;
    std::string _uid;
    iVec2 _coor;
};

template <class T>
class Quad {
public:
    Quad(const iVec2 &_left_top, const iVec2 &_right_bottom) : _child(false) {
        this->_left_top = _left_top;
        this->_right_bottom = _right_bottom;

        // 最后一层,要存节点
        if (abs(_left_top.x - _right_bottom.x) < 1 &&
            abs(_left_top.y - _right_bottom.y) < 1) {
            _container = std::make_shared<QuadContainer<T>>();

            _child = true;
        }
    }

    QuadNode<T> insert(const iVec2 &_where, const T &val) {
        if (!inside(_where)) {
            return QuadNode<T>{};
        }

        if (_child) {
            auto uid = _container->store(val);
            return QuadNode<T>{_container, uid, _where};
        }

        int idx = getAndNewQuads(_where);

        return _quads[idx]->insert(_where, val);
    }

    void visitInRect(
        const iVec2 &_left_top, const iVec2 &_right_bottom,
        const std::function<void(const iVec2 &cor, T &val)> &func) const {
        // 判断界外
        if (this->_right_bottom.x < _left_top.x ||
            this->_left_top.x > _right_bottom.x ||
            this->_right_bottom.y > _left_top.y ||
            this->_left_top.y < _right_bottom.y) {
            return;
        }

        if (_child) {
            for (auto &it : _container->_container) {
                func(_left_top, it.second);
            }
        } else {
            for (int i = 1; i <= 4; ++i) {
                if (_quads[i].get()) {
                    _quads[i]->visitInRect(_left_top, _right_bottom, func);
                }
            }
        }
    }

    bool inside(const iVec2 &cor) {
        if (cor.x >= _left_top.x && cor.x <= _right_bottom.x &&
            cor.y >= _right_bottom.y && cor.y <= _left_top.y) {
            return true;
        }
        return false;
    }

private:
    int getAndNewQuads(const iVec2 &_where) {
        int x_mid = (_left_top.x + _right_bottom.x) >> 1;
        int y_mid = (_left_top.y + _right_bottom.y) >> 1;

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
        if (_quads[pp].get() == nullptr) {
            if (pp == 1) {
                _quads[1] =
                    std::make_shared<Quad<T>>(iVec2{x_mid + 1, _left_top.y},
                                              iVec2{_right_bottom.x, y_mid + 1});
            } else if (pp == 2) {
                _quads[2] = std::make_shared<Quad<T>>(
                    iVec2{_left_top.x, _left_top.y}, iVec2{x_mid, y_mid + 1});
            } else if (pp == 3) {
                _quads[3] = std::make_shared<Quad<T>>(
                    iVec2{_left_top.x, y_mid}, iVec2{x_mid, _right_bottom.y});
            } else if (pp == 4) {
                _quads[4] = std::make_shared<Quad<T>>(
                    iVec2{x_mid + 1, y_mid},
                    iVec2{_right_bottom.x, _right_bottom.y});
            }
        }

        return pp;
    }
    iVec2 _left_top;
    iVec2 _right_bottom;

    bool _child;

    // 最后一层,存元素的容器
    std::shared_ptr<QuadContainer<T>> _container;

    // 四个子象限 1,2,3,4. 0不用
    std::shared_ptr<Quad<T>> _quads[5];
};

#endif