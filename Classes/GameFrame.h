#ifndef __GAME_FRAME_H__
#define __GAME_FRAME_H__

#include <memory>
#include <string>
#include <vector>
using namespace std;

#include "Connection.h"
#include "json.h"

class GameFrame;

enum GameActType {
    act_unknow,
    act_move_start,
    act_move_stop,
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

    void receiveFrameFromServer(const json& frame_event);

    // 生成next_frame的json
    json generateJsonOfNextFrame(const string& connection_uid);

    //生成新的next_frame
    void newNextFrame();

    // 是否有新的从服务器回来的帧
    bool hasNewFrame();

    // 获得下一帧
    GameFrame* getNextFrame();

    // 推动作到next_frame
    void pushGameAct(const GameAct& act);

private:
    shared_ptr<GameFrame> _next_frame;

    // 从服务器接收回来的帧
    vector<GameFrame> frames;

    int head, tail;
};

class GameFrame {
public:
    vector<GameAct> actions;
};

#endif