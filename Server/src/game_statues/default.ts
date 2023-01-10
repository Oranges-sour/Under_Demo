
import { Connection } from "../connection";
import { EventBag } from "../event";
import { Game } from "../game";
import { IGameStatue } from "../game_statue";

import { GameStatue_Start } from "./start";

class GameStatue_Default implements IGameStatue {
    process_event(game: Game, event: EventBag): void {
        if (event.type == "chat_connection_say") {
            let ev = new EventBag();
            ev = event;
            ev.type = "chat_server_send_back";
            game.visit_host(function (connection) {
                connection.push_event(ev);
            });
            game.visit_all_player(function (connection) {
                connection.push_event(ev);
            });
            return;
        }
        if (event.type == "quit_game") {
            let uid = event.info_1;
            let is_host = false;
            game.visit_host(function (connection) {
                if (connection === null) {
                    return;
                }
                if (connection.get_uid() == uid) {
                    is_host = true;
                }
            });

            //是房主，整个游戏房间都要被销毁
            if (is_host) {
                let event = new EventBag();
                event.type = "quit_game";
                game.visit_host(function (connection) {
                    connection.push_event(event);
                });
                game.visit_all_player(function (connection) {
                    connection.push_event(event);
                });
                //防止在被移除之前有其他连接加入
                game.set_uid("bad_uid");
            }
            //不是房主，自行退出即可
            if (!is_host) {
                let event = new EventBag();
                event.type = "quit_game";
                game.visit_all_player(function (connection) {
                    if (connection.get_uid() == uid) {
                        connection.push_event(event);
                    }
                });

                //给其他人通知，有玩家退出了
                let event1 = new EventBag();
                event1.type = "player_quit_game";
                event1.info_1 = uid;
                game.visit_host(function (connection) {
                    if (connection === null) {
                        return;
                    }
                    connection.push_event(event1);
                });
                game.visit_all_player(function (connection) {
                    connection.push_event(event1);
                });
            }
        }
        if (event.type == "start_game") {
            let is_host = true;
            game.visit_host(function (connection) {
                if (connection === null) {
                    is_host = false;
                    return;
                }
                if (connection.get_uid() != event.info_2) {
                    is_host = false;
                }
            });
            if (!is_host) {
                return;
            }

            let ev = new EventBag();
            ev.type = "start_game";
            ev.info_1 = event.info_1;//seed
            ev.info_3 = String(game.get_player_cnt());

            let arr = new Array<string>();

            game.visit_host(function (connection) {
                arr.push(connection.get_uid());
            });
            game.visit_all_player(function (connection) {
                arr.push(connection.get_uid());
            });

            ev.info_4 = arr;

            game.visit_host(function (connection) {
                ev.info_2 = "1";//is_host
                connection.push_event(ev);
            });
            game.visit_all_player(function (connection) {
                ev.info_2 = "0";//not_host
                connection.push_event(ev);
            });

            game.swtich_statue(new GameStatue_Start());
        }
    }
    update(game: Game): void {
    }
    get_statue_name(): string {
        return "GameStatue_Default";
    }
}

export { GameStatue_Default }