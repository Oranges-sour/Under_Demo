#ifndef __GAME_FRAME_H__
#define __GAME_FRAME_H__

#include <memory>
#include <queue>
#include <string>
#include <vector>
using namespace std;

#include "Connection.h"
#include "json.h"

class GameFrame;
class GameWorld;

enum GameActType {
    act_unknow,
    act_move_start,
    act_move_stop,
    act_attack,
    act_position_force_set,
};

class GameAct {
public:
    GameAct()
        : uid("bad_id"),
          type(act_unknow),
          param1(0),
          param2(0),
          param3(0),
          param4(0) {}

    string uid;
    GameActType type;
    float param1;
    float param2;
    float param3;
    float param4;
};

class GameFrameManager {
public:
    GameFrameManager();

    void init(GameWorld* game_world);

    void release();

    void noitce(const json& frame_event);

    void update();

    // 推动作到next_frame
    void pushGameAct(const GameAct& act, bool role_back = true /*是否作用到自己*/);

private:
    shared_ptr<GameFrame> generate_frame(const json& event);

    json generate_json(shared_ptr<GameFrame> game_frame);

private:
    GameWorld* _game_world;

    shared_ptr<GameFrame> _next_frame;
};

class GameFrame {
public:
    vector<GameAct> actions;
};

#endif