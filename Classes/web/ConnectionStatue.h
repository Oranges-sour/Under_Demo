#ifndef __CONNECTION_STATUE_H__
#define __CONNECTION_STATUE_H__

#if 0

#include <string>
using namespace std;

#include "utility/json/json.h"

class Connection;

class ConnectionStatue {
public:
    virtual void processMessage(Connection* connection,
                                const string& message) = 0;
    virtual void update(Connection* connection, int interval_ms) = 0;
    virtual void processEvent(Connection* connection, const json& event) = 0;
};

class ConnectionStatue_Default : public ConnectionStatue {
public:
    ConnectionStatue_Default() { _call_once = false; }
    virtual void processMessage(Connection* connection,
                                const string& message) override;
    virtual void update(Connection* connection, int interval_ms) override;
    virtual void processEvent(Connection* connection,
                              const json& event) override {}

private:
    bool _call_once;
};

class ConnectionStatue_Normal : public ConnectionStatue {
public:
    virtual void processMessage(Connection* connection,
                                const string& message) override;
    virtual void update(Connection* connection, int interval_ms) override;

    virtual void processEvent(Connection* connection,
                              const json& event) override;
};

class ConnectionStatue_InGame : public ConnectionStatue {
public:
    virtual void processMessage(Connection* connection,
                                const string& message) override;
    virtual void update(Connection* connection, int interval_ms) override;

    virtual void processEvent(Connection* connection,
                              const json& event) override;
};

class ConnectionStatue_StartGame : public ConnectionStatue {
public:
    virtual void processMessage(Connection* connection,
                                const string& message) override;
    virtual void update(Connection* connection, int interval_ms) override;

    virtual void processEvent(Connection* connection,
                              const json& event) override;
};

#endif

#endif