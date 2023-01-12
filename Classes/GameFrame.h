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

    // ����next_frame��json
    json generateJsonOfNextFrame(const string& connection_uid);

    //�����µ�next_frame
    void newNextFrame();

    // �Ƿ����µĴӷ�����������֡
    bool hasNewFrame();

    // �����һ֡
    GameFrame* getNextFrame();

    // �ƶ�����next_frame
    void pushGameAct(const GameAct& act);

private:
    shared_ptr<GameFrame> _next_frame;

    // �ӷ��������ջ�����֡
    vector<GameFrame> frames;

    int head, tail;
};

class GameFrame {
public:
    vector<GameAct> actions;
};

#endif