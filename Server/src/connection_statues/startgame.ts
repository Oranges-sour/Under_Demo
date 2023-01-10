import { IConnectionStatue } from "../connection_statue";
import { Connection } from "../connection";
import { Tools } from "../tools";
import { ServerDefault } from "../tools";

import { EventBag } from "../event";
import { ConnectionStatue_Normal } from "./normal";

class ConnectionStatue_StartGame implements IConnectionStatue {
    process_message(connection: Connection, message: string): void {
        let obj = Tools.parse_JSON_safe(message);
        if (obj === null) {
            return;
        }

        class MSG {
            type: string;
            info_1: string;
            info_2: string;
            info_3: string;
            info_4: string;
            constructor() {
                this.type = "";
                this.info_1 = "";
                this.info_2 = "";
                this.info_3 = "";
                this.info_4 = "";
            }
        }

        let msg = obj as MSG;
        if (msg.type == "player_move_start") {

            let ev = new EventBag();
            ev.type = "player_move_start";
            ev.info_1 = connection.get_uid();
            ev.info_2 = String(msg.info_2);//x
            ev.info_3 = String(msg.info_3);//y

            Tools.log(`${msg.info_2}}`);

            let game = connection.get_game();
            if (game === null) {
                return;
            }
            Tools.log("receive player_move_start");
            game.push_event(ev);
            return;
        }
        if (msg.type == "player_move_stop") {

            let ev = new EventBag();
            ev.type = "player_move_stop";
            ev.info_1 = connection.get_uid();
            ev.info_2 = String(msg.info_2);//x
            ev.info_3 = String(msg.info_3);//y

            let game = connection.get_game();
            if (game === null) {
                return;
            }
            Tools.log("receive player_move_stop");
            game.push_event(ev);
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
        if (event.type == "player_move_start") {
            let e = {
                type: "player_move_start",
                uid: event.info_1,
                x: event.info_2,
                y: event.info_3,
            }
            connection.send_message(e);
        }
        if (event.type == "player_move_stop") {
            let e = {
                type: "player_move_stop",
                uid: event.info_1,
                x: event.info_2,
                y: event.info_3,
            }
            connection.send_message(e);
        }

    }

    update(connection: Connection, interval_ms: number): void { }
}

export { ConnectionStatue_StartGame }