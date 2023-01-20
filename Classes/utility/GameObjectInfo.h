#ifndef __GAME_OBJECT_INFO_H__
#define __GAME_OBJECT_INFO_H__

#include <map>
#include <string>
using namespace std;

#include "utility/json/json.h"

class GameObjectInfo {
public:
    static GameObjectInfo* instance();

private:
    static GameObjectInfo* _instance;

public:
    void load(const string& file);

    const json& get(const string& key);

private:
    map<string, json> _map;
};

#endif