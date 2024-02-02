#include "Connection.h"

#if 0

#include "ConnectionStatue.h"
#include "cocos2d.h"

Connection* Connection::_instance = new (std::nothrow) Connection();

Connection* Connection::instance() { return Connection::_instance; }

Connection::Connection() : _is_open(false), _is_error(false), _start(true) {
    this->_uid = "";

    this->_thread = make_shared<thread>([&]() {
        while (_start) {
            unique_lock<mutex> lock(this->_statue_event_mutex);
            _cv.wait(lock);

            while (!_statue_event_queue.empty()) {
                auto& t = _statue_event_queue.front();

                this->_statue->processEvent(this, t);

                _statue_event_queue.pop();
            }
        }
    });
}

Connection::~Connection() {
    if (this->isOpen()) {
        _ws->close();
    }

    _start = false;

    _cv.notify_all();
    _thread->join();
}

bool Connection::open(const string& ip) {
    this->_ws = make_shared<WebSocket>();
    if (!_ws->init(*this, ip)) {
        return false;
    }
    _is_error = false;
    this->_statue = make_shared<ConnectionStatue_Default>();
    return true;
}

void Connection::close() {
    if (_ws) {
        _ws->close();
    }
}

void Connection::onOpen(WebSocket* ws) { _is_open = true; }

void Connection::onMessage(WebSocket* ws, const WebSocket::Data& data) {
    this->processMessage(data.bytes);
}

void Connection::onClose(WebSocket* ws) { _is_open = false; }

void Connection::onError(WebSocket* ws, const WebSocket::ErrorCode& error) {
    _is_error = true;
    CCLOG("Connection onError: %d", (int)error);
    this->close();
}

void Connection::processMessage(const string& message) {
    this->_statue->processMessage(this, message);
}

void Connection::update(int interval_ms) {
    if (!isOpen()) {
        return;
    }

    this->_statue->update(this, interval_ms);

    unique_lock<mutex> loc(_listener_event_mutex);
    while (!_listener_event_queue.empty()) {
        auto& t = _listener_event_queue.front();

        for (auto& it : _event_listener) {
            it.second->notice(t);
        }
        _listener_event_queue.pop();
    }
}

void Connection::pushListenerEvent(const json& event) {
    unique_lock<mutex> loc(_listener_event_mutex);
    _listener_event_queue.push(event);
}

void Connection::pushStatueEvent(const json& event) {
    unique_lock<mutex> lock(_statue_event_mutex);

    _statue_event_queue.push(event);

    _cv.notify_one();
}

#endif