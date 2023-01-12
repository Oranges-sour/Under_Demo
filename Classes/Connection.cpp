#include "Connection.h"

#include "ConnectionStatue.h"

bool Connection::init(const string& ip) {
    _is_ready = false;
    _is_init = false;

    this->_ws = make_shared<WebSocket>();
    if (!_ws->init(*this, ip)) {
        return false;
    }
    _is_init = true;

    this->_statue = make_shared<ConnectionStatue_Default>();

    this->_thread = make_shared<thread>([&]() {
        while (true) {
            unique_lock<mutex> lock(this->_mutex);
            _cv.wait(lock);
            if (statue_event_queue.empty()) {
                break;
            }

            while (!statue_event_queue.empty()) {
                auto& t = statue_event_queue.front();

                this->_statue->processEvent(this, t);

                statue_event_queue.pop();
            }
        }
    });
    return true;
}

void Connection::onOpen(WebSocket* ws) {
    if (!_is_init) {
        return;
    }
}

void Connection::onMessage(WebSocket* ws, const WebSocket::Data& data) {
    if (!_is_init) {
        return;
    }
    this->process_message(data.bytes);
}

void Connection::onClose(WebSocket* ws) {
    _is_init = false;
    _is_ready = false;
}

void Connection::onError(WebSocket* ws, const WebSocket::ErrorCode& error) {
    _is_init = false;
    _is_ready = false;
    _ws->close();
}

void Connection::process_message(const string& message) {
    if (!_is_init) {
        return;
    }
    this->_statue->processMessage(this, message);
}

void Connection::update(int interval_ms) {
    if (!_is_init) {
        return;
    }

    if (this->_ws->getReadyState() == WebSocket::State::OPEN) {
        _is_ready = true;
    }

    if (!is_ready()) {
        return;
    }
    this->_statue->update(this, interval_ms);

    while (!listener_event_queue.empty()) {
        auto& t = listener_event_queue.front();

        for (auto& it : event_listener) {
            it.second->notice(t);
        }

        listener_event_queue.pop();
    }
}

void Connection::push_statueEvent(const json& event) {
    unique_lock<mutex> lock(_mutex);

    statue_event_queue.push(event);
    _cv.notify_one();
}
