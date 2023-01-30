#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <condition_variable>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
using namespace std;

#include "cocos/network/WebSocket.h"
using namespace cocos2d;
using namespace cocos2d::network;

#include "utility/json/json.h"

class ConnectionStatue;

class ConnectionEventListener {
public:
    ConnectionEventListener(const function<void(const json &)> &call_back)
        : call_back(call_back) {}
    void notice(const json &event) { this->call_back(event); }

private:
    function<void(const json &)> call_back;
};

class Connection : public WebSocket::Delegate {
public:
    static Connection* _instance;
    static Connection* instance();

public:
    Connection();
    ~Connection();

    bool open(const string &ip);

    void close();

    virtual void onOpen(WebSocket *ws);
    virtual void onMessage(WebSocket *ws, const WebSocket::Data &data);
    virtual void onClose(WebSocket *ws);
    virtual void onError(WebSocket *ws, const WebSocket::ErrorCode &error);

    void processMessage(const string &message);
    void update(int interval_ms);

    bool isOpen() { return _is_open; }

    bool isError() { return _is_error; }

    void switchStatue(shared_ptr<ConnectionStatue> newStatue) {
        _statue = newStatue;
    }

    void sendMessage(const string &message) { _ws->send(message); }

    void addEventListener(shared_ptr<ConnectionEventListener> event,
                                const string &key) {
        _event_listener.insert({key, event});
    }

    void removeEventListener(const string &key) { _event_listener.erase(key); }

    // 将事件推给监听者
    void pushListenerEvent(const json &event);

    // 将事件推给状态机
    void pushStatueEvent(const json &event);

    void setUID(const string &newid) { _uid = newid; }

    string getUID() { return _uid; }

private:
    string _uid;
    bool _is_open;
    bool _is_error;

    bool _start;

    // 给监听者的事件队列（监听者处理事件
    queue<json> _listener_event_queue;

    // 给连接状态机的时间队列（状态机处理事件
    queue<json> _statue_event_queue;

    map<string, shared_ptr<ConnectionEventListener>> _event_listener;

    shared_ptr<ConnectionStatue> _statue;
    shared_ptr<WebSocket> _ws;

    shared_ptr<thread> _thread;
    mutex _statue_event_mutex;
    mutex _listener_event_mutex;
    condition_variable _cv;
};

#endif