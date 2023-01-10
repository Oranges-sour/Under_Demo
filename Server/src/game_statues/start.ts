import { Connection } from "../connection";
import { EventBag } from "../event";
import { Game } from "../game";
import { IGameStatue } from "../game_statue";
import { Tools } from "../tools";

class GameStatue_Start implements IGameStatue {
    process_event(game: Game, event: EventBag): void {
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

        if (event.type == "player_move_start") {
            let ev = new EventBag();

            ev.type = "player_move_start";
            ev.info_1 = event.info_1;//uid
            ev.info_2 = event.info_2;//x
            ev.info_3 = event.info_3;//y

            game.visit_host(function (connection) {
                connection.push_event(ev);
            });
            game.visit_all_player(function (connection) {
                connection.push_event(ev);
            });
        }
        if (event.type == "player_move_stop") {
            let ev = new EventBag();

            ev.type = "player_move_stop";
            ev.info_1 = event.info_1;//uid
            ev.info_2 = event.info_2;//x
            ev.info_3 = event.info_3;//y

            game.visit_host(function (connection) {
                connection.push_event(ev);
            });
            game.visit_all_player(function (connection) {
                connection.push_event(ev);
            });
        }

    }
    update(game: Game): void {
    }
    get_statue_name(): string {
        return "GameStatue_Start";
    }
}

export { GameStatue_Start };