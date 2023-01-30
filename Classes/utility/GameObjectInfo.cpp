#include "GameObjectInfo.h"

#include "cocos2d.h"
USING_NS_CC;

#include <sstream>

GameObjectInfo* GameObjectInfo::_instance = new GameObjectInfo();

GameObjectInfo* GameObjectInfo::instance() { return _instance; }

void GameObjectInfo::load(const string& file) {
    auto inst = FileUtils::getInstance();
    if (!inst->isFileExist(file)) {
        assert(false);
        return;
    }
    auto str = inst->getStringFromFile(file);

    stringstream ss;
    ss << str;

    json js = json::parse(ss);

    vector<json> vec = js["objects"];
    for (auto& it : vec) {
        string name = it["name"];
        json info = it["info"];
        _map.insert({name, info});
    }
}

const json& GameObjectInfo::get(const string& key) {
    auto iter = _map.find(key);
    if (iter == _map.end()) {
        return _BAD_JSON;
    }
    return iter->second;
}
