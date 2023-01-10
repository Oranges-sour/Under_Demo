import { IConnectionStatue } from "../connection_statue";
import { Connection } from "../connection";
import { Tools } from "../tools";
import { ServerDefault } from "../tools";
import { Game } from "../game";
import { game_pool } from "../game";
import { ConnectionStatue_InGame } from "./ingame";
import { GameStatue_Default } from "../game_statues/default";
import { EventBag } from "../event";

class ConnectionStatue_Normal implements IConnectionStatue {
    time: number;

    constructor() {
        this.time = 0;
    }

    process_message(connection: Connection, message: string): void {
        const COMING_MSG_TYPE_CREATE_GAME = "create_game";
        const COMING_MSG_TYPE_JOIN_GAME = "join_game";


        let msg = transfor_message(message);
        if (msg === undefined) {
            return;
        }

        if (msg.type === undefined ||
            msg.info === undefined) {
            return;
        }

        if (msg.type == COMING_MSG_TYPE_CREATE_GAME) {
            on_create_game(connection, msg);
            return;
        }

        if (msg.type == COMING_MSG_TYPE_JOIN_GAME) {
            on_join_game(connection, msg);
            return;
        }


    }
    process_event(connection: Connection, event: EventBag): void {
        if (event.type == "connection_on_close") {
            connection.set_is_on_close(true);
            connection.set_is_alive(false);
        }
    }
    update(connection: Connection, interval_ms: number): void {
        this.time += interval_ms;

        //500ms 发送当前的game状态
        if (this.time >= 500) {
            this.time = 0;

            let game_statue = {
                type: "game_statue",
                games: new Array<{
                    uid: string,
                    description: string,
                }>(),
            };

            game_pool.visit(function (game) {
                game_statue.games.push(
                    {
                        uid: game.get_uid(),
                        description: game.get_description()
                    }
                );
            });

            connection.send_message(game_statue);

        }
    }
}

function transfor_message(message: string) {
    let obj = Tools.parse_JSON_safe(message);
    if (obj === null) {
        return undefined;
    }

    const MSG = {
        type: "",
        info: "",
    }

    let msg = obj as typeof MSG;

    return msg;
}

function on_join_game(connection: Connection, msg: { type: string, info: string }) {
    const SENDING_MSG_TYPE_JOIN_GAME = "join_game_result";
    const STATUE_SUCCESS = "success";
    const STATUE_FAILED = "failed";

    //遍历寻找房间
    let game_find: Game = null!;
    game_pool.visit(function (game) {
        if (game.get_uid() == msg.info) {
            game_find = game;
        }
    });

    //未找到房间
    if (game_find == null) {
        let join_failed = {
            type: SENDING_MSG_TYPE_JOIN_GAME,
            statue: STATUE_FAILED,
            info: "未找到对应uid的game房间",
        }
        connection.send_message(join_failed);
        return;
    }

    game_find.add_player(connection);
    connection.switch_statue(new ConnectionStatue_InGame());

    let join_success = {
        type: SENDING_MSG_TYPE_JOIN_GAME,
        statue: STATUE_SUCCESS,
        info: game_find.get_uid(),
    }
    connection.send_message(join_success);
}

function on_create_game(connection: Connection, msg: { type: string, info: string }) {
    const SENDING_MSG_TYPE_CREATE_GAME = "create_game_result";
    const STATUE_SUCCESS = "success";

    let game = new Game(connection, msg.info);
    game_pool.insert(game);
    connection.switch_statue(new ConnectionStatue_InGame());

    let create_success = {
        type: SENDING_MSG_TYPE_CREATE_GAME,
        statue: STATUE_SUCCESS,
        info: "",
    }
    connection.send_message(create_success);

    Tools.log(`Create game success. uid: ${game.get_uid()} `);
}

export { ConnectionStatue_Normal };