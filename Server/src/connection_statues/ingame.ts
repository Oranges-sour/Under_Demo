import { IConnectionStatue } from "../connection_statue";
import { Connection } from "../connection";
import { Tools } from "../tools";
import { ServerDefault } from "../tools";

import { EventBag } from "../event";
import { ConnectionStatue_Normal } from "./normal";
import { game_pool } from "../game";

import { ConnectionStatue_StartGame } from "./startgame";

class ConnectionStatue_InGame implements IConnectionStatue {
    process_message(connection: Connection, message: string): void {

        let obj = Tools.parse_JSON_safe(message);
        if (obj === null) {
            return;
        }

        class MSG {
            type: string;
            info_1: string;
            info_2: string;
            constructor() {
                this.type = "";
                this.info_1 = "";
                this.info_2 = "";
            }

        }

        let msg = obj as MSG;
        if (msg.type === undefined ||
            msg.info_1 === undefined ||
            msg.info_2 === undefined) {
            return;
        }

        //自己的消息，要广播到游戏房间
        if (msg.type == "chat_connection_say") {
            let game = connection.get_game();
            if (game === null) {
                return;
            }

            let event = new EventBag();
            event.type = "chat_connection_say";
            event.info_1 = msg.info_1;
            event.info_2 = connection.get_uid();
            game.push_event(event);
            return;
        }

        if (msg.type == "quit_game") {
            let game = connection.get_game();
            if (game === null) {
                return;
            }

            let event = new EventBag();
            event.type = "quit_game";
            event.info_1 = connection.get_uid();
            game.push_event(event);
            return;
        }
        if (msg.type == "start_game") {

            let game = connection.get_game();
            if (game === null) {
                return;
            }
            Tools.log("Try start game.");

            let event = new EventBag();
            event.type = "start_game";
            event.info_1 = msg.info_1;//seed
            event.info_2 = connection.get_uid();

            game.push_event(event);
            return;
        }
    }





    process_event(connection: Connection, event: EventBag): void {
        if (event.type == "connection_on_close") {
            connection.set_is_on_close(true);

            let game = connection.get_game();
            if (game === null) {
                connection.set_is_alive(false);
                return;
            }

            let event = new EventBag();
            event.type = "quit_game";
            event.info_1 = connection.get_uid();
            game.push_event(event);
            return;
        }

        //游戏房间发回来的消息，发回客户端
        if (event.type == "chat_server_send_back") {
            let e = {
                type: "chat_new_message",
                uid: event.info_2,
                info: event.info_1,
            }
            connection.send_message(e);
            return;
        }
        //收到事件，可以退出房间
        if (event.type == "quit_game") {
            let game = connection.get_game();
            game.remove_player(connection);

            connection.switch_statue(new ConnectionStatue_Normal());

            //如果是在断开连接，移除玩家后此连接也要被移除
            if (connection.get_is_on_close() == true) {
                connection.set_is_alive(false);
            }

            let e = {
                type: "quit_game_result",
                statue: "success",
                info: "",
            };
            connection.send_message(e);
            return;
        }

        //收到事件，有玩家退出房间
        if (event.type == "player_quit_game") {
            let e = {
                type: "player_quit_game",
                info: event.info_1,//uid
            };
            connection.send_message(e);
            return;
        }
        if (event.type == "start_game") {
            let e = {
                type: "start_game",
                seed: event.info_1,
                is_host: event.info_2,
                player_cnt: event.info_3,
                player_uid: event.info_4,
            };
            connection.send_message(e);

            connection.switch_statue(new ConnectionStatue_StartGame());
        }
    }
    update(connection: Connection, interval_ms: number): void {
    }
}

export { ConnectionStatue_InGame };