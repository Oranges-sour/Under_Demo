import { IConnectionStatue } from "../connection_statue";
import { Connection } from "../connection";
import { Tools } from "../tools";
import { ServerDefault } from "../tools";

import { EventBag } from "../event";
import { ConnectionStatue_Normal } from "./normal";

class Frame_MSG {
    type: string;
    frame: {
        cnt: number,
        player: string,
        actions: Array<{
            uid: string,
            type: number,
            param1: number,
            param2: number,
            param3: number,
            param4: number,
        }>,
    }
    constructor() {
        this.type = "";
        this.frame = { cnt: -1, player: "bad_id", actions: [] };
    }
}

class ConnectionStatue_StartGame implements IConnectionStatue {
    process_message(connection: Connection, message: string): void {
        let obj = Tools.parse_JSON_safe(message);
        if (obj === null) {
            return;
        }

        class MSG {
            type: string;
            constructor() {
                this.type = "";
            }
        }

        let msg = obj as MSG;
        if (msg.type == "frame") {

            let frame_msg = obj as Frame_MSG;
            let game = connection.get_game();
            game.push_event(frame_msg as any as EventBag);
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
        
        if(event.type == "frame") {
            let frame = event as any as Frame_MSG;
            connection.send_message(frame);
            return;
        }

    }

    update(connection: Connection, interval_ms: number): void { }
}

export { ConnectionStatue_StartGame }