#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <functional>
#include <map>
#include <memory>
#include <queue>
#include <string>
using namespace std;

#include "cocos/network/WebSocket.h"
using namespace cocos2d;
using namespace cocos2d::network;

#include "json.h"
using json = nlohmann::json;

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
    Connection() : _is_ready(false), _is_init(false) { uid = ""; }
    ~Connection() { _ws->close(); }

    bool init(const string &ip);

    virtual void onOpen(WebSocket *ws);
    virtual void onMessage(WebSocket *ws, const WebSocket::Data &data);
    virtual void onClose(WebSocket *ws);
    virtual void onError(WebSocket *ws, const WebSocket::ErrorCode &error);

    void process_message(const string &message);
    void update(int interval_ms);

    bool is_ready() { return _is_ready; }

    void switchStatue(shared_ptr<ConnectionStatue> newStatue) {
        _statue = newStatue;
    }

    void sendMessage(const string &message) { _ws->send(message); }

    void regeist_event_listener(shared_ptr<ConnectionEventListener> event,
                                const string &key) {
        event_listener.insert({key, event});
    }

    void remove_event_listener(const string &key) { event_listener.erase(key); }

    // ���¼��Ƹ�������
    void push_listenerEvent(const json &event) {
        listener_event_queue.push(event);
    }

    // ���¼��Ƹ�״̬��
    void push_statueEvent(const json &event) { statue_event_queue.push(event); }

    void set_uid(const string &newid) { uid = newid; }

    string get_uid() { return uid; }

private:
    string uid;
    bool _is_init;
    bool _is_ready;

    // �������ߵ��¼����У������ߴ����¼�
    queue<json> listener_event_queue;

    // ������״̬����ʱ����У�״̬�������¼�
    queue<json> statue_event_queue;

    map<string, shared_ptr<ConnectionEventListener>> event_listener;

    shared_ptr<ConnectionStatue> _statue;
    shared_ptr<WebSocket> _ws;
};

#endif