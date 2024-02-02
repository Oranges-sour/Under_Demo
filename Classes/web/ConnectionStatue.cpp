#include "ConnectionStatue.h"

#if 0 

#include <vector>

#include "cocos2d.h"
#include "utility/Tools.h"
#include "web/Connection.h"
using namespace std;

void ConnectionStatue_Default::processMessage(Connection* connection,
                                              const string& message) {
    json js;

    try {
        js = json::parse(message);
    } catch (exception e) {
        return;
    }

    if (!js.contains("type") || !js.contains("statue") ||
        !js.contains("info")) {
        return;
    }

    string type = js["type"];
    string statue = js["statue"];
    string info = js["info"];

    if (type != "client_check_result") {
        return;
    }

    if (statue == "failed") {
        CCLOG("ConnectionStatue_Default: failed >> %s", info.c_str());
        return;
    }

    // ³É¹¦ÁË
    connection->switchStatue(make_shared<ConnectionStatue_Normal>());
}

void ConnectionStatue_Default::update(Connection* connection, int interval_ms) {
    if (!_call_once) {
        _call_once = true;

        string uid = Tools::randomString(8);

        connection->setUID(uid);

        json msg;
        msg["type"] = "client_check";
        msg["version"] = "v0.3.4";
        msg["uid"] = uid;
        msg["security_key"] = "1kd73b0pmz37ru91saqkm0286ti61ckg";

        connection->sendMessage(to_string(msg));
    }
}

///////////////////////////////////////////////////////////////////////////

void ConnectionStatue_Normal::processMessage(Connection* connection,
                                             const string& message) {
    json js;

    try {
        js = json::parse(message);
    } catch (exception e) {
        return;
    }

    if (!js.contains("type")) {
        return;
    }

    string type = js["type"];
    if (type == "game_statue") {
        vector<json> vec = js["games"];

        json event;
        event["type"] = "game_statue";
        event["games"] = vec;
        connection->pushListenerEvent(event);
        return;
    }

    if (type == "create_game_result" || type == "join_game_result") {
        if (js["statue"] == "success") {
            connection->switchStatue(make_shared<ConnectionStatue_InGame>());
        }

        json event;
        event["type"] = type;
        event["statue"] = js["statue"];
        event["info"] = js["info"];

        connection->pushListenerEvent(event);
        return;
    }
}

void ConnectionStatue_Normal::update(Connection* connection, int interval_ms) {}

void ConnectionStatue_Normal::processEvent(Connection* connection,
                                           const json& event) {
    if (!event.contains("type")) {
        return;
    }

    string type = event["type"];
    if (type == "create_game") {
        json js;
        js["type"] = "create_game";
        js["info"] = event["info"];

        connection->sendMessage(to_string(js));
        return;
    }
    if (type == "join_game") {
        json js;
        js["type"] = "join_game";
        js["info"] = event["info"];
        connection->sendMessage(to_string(js));
        return;
    }
}

///////////////////////////////////////////////////////////////////////////

void ConnectionStatue_InGame::processMessage(Connection* connection,
                                             const string& message) {
    json js;

    try {
        js = json::parse(message);
    } catch (exception e) {
        return;
    }

    if (!js.contains("type")) {
        return;
    }

    string type = js["type"];
    if (type == "chat_new_message") {
        json event;
        event["type"] = "chat_new_message";
        event["uid"] = js["uid"];
        event["info"] = js["info"];

        connection->pushListenerEvent(event);
        return;
    }
    if (type == "quit_game_result") {
        string statue = js["statue"];
        if (statue == "success") {
            json event;
            event["type"] = "quit_game_result";
            event["statue"] = "success";

            connection->pushListenerEvent(event);

            connection->switchStatue(make_shared<ConnectionStatue_Normal>());
        }
    }
    if (type == "start_game") {
        connection->switchStatue(make_shared<ConnectionStatue_StartGame>());

        json event;
        event["type"] = "start_game";
        event["seed"] = js["seed"];
        event["is_host"] = js["is_host"];
        event["player_cnt"] = js["player_cnt"];
        event["player_uid"] = js["player_uid"];

        connection->pushListenerEvent(event);
    }
}

void ConnectionStatue_InGame::update(Connection* connection, int interval_ms) {}

void ConnectionStatue_InGame::processEvent(Connection* connection,
                                           const json& event) {
    if (!event.contains("type")) {
        return;
    }

    string type = event["type"];
    if (type == "chat_connection_say") {
        json js;
        js["type"] = "chat_connection_say";
        js["info_1"] = event["info_1"];
        js["info_2"] = event["info_2"];

        connection->sendMessage(to_string(js));
        return;
    }

    if (type == "quit_game") {
        json js;
        js["type"] = "quit_game";
        js["info_1"] = event["info_1"];
        js["info_2"] = event["info_2"];

        connection->sendMessage(to_string(js));
        return;
    }
    if (type == "start_game") {
        json js;
        js["type"] = "start_game";
        js["info_1"] = event["info_1"];  // seed
        js["info_2"] = "";

        connection->sendMessage(to_string(js));
        return;
    }
}

//////////////////////////////////////////////////////////////////

void ConnectionStatue_StartGame::processMessage(Connection* connection,
                                                const string& message) {
    json js;

    try {
        js = json::parse(message);
    } catch (exception e) {
        return;
    }

    if (!js.contains("type")) {
        return;
    }

    string type = js["type"];

    if (type == "quit_game_result") {
        string statue = js["statue"];
        if (statue == "success") {
            json event;
            event["type"] = "quit_game_result";
            event["statue"] = "success";

            connection->pushListenerEvent(event);

            connection->switchStatue(make_shared<ConnectionStatue_Normal>());
        }
    }

    if (type == "frame") {
        connection->pushListenerEvent(js);
    }

    if (type == "ping") {
        connection->pushListenerEvent(js);
    }
}

void ConnectionStatue_StartGame::update(Connection* connection,
                                        int interval_ms) {}

void ConnectionStatue_StartGame::processEvent(Connection* connection,
                                              const json& event) {
    string type = event["type"];
    if (type == "frame") {
        connection->sendMessage(to_string(event));
    }
    if (type == "ping") {
        connection->sendMessage(to_string(event));
    }
}

#endif