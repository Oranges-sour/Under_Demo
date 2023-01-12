import { Connection } from "../connection";
import { EventBag } from "../event";
import { Game } from "../game";
import { IGameStatue } from "../game_statue";
import { Tools } from "../tools";

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

class GameStatue_Start implements IGameStatue {

    new_frame: boolean;
    frame: Frame_MSG;
    frame_player: number;
    constructor() {
        this.new_frame = true;
        this.frame_player = 0;
        this.frame = new Frame_MSG();
    }

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

        if (event.type == "frame") {

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
                    this.type = "frame";
                    this.frame = { cnt: -1, player: "bad_id", actions: [] };
                }
            }

            let frame = event as any as Frame_MSG;

            if (this.new_frame) {
                this.frame = new Frame_MSG();
                this.frame.frame.cnt = frame.frame.cnt;

                this.new_frame = false;
                this.frame_player = 0;
            }

            let that = this;
            frame.frame.actions.forEach(function (act) {
                that.frame.frame.actions.push(act);
            });
            this.frame_player += 1;


            //收集到了所有的帧，下发
            if (this.frame_player == game.get_player_cnt()) {
                this.frame_player = 0;

                //Tools.log("Next frame: " + JSON.stringify(that.frame));
                
                game.visit_host(function (c) {
                    c.push_event(that.frame as any as EventBag);
                });
                game.visit_all_player(function (c) {
                    c.push_event(that.frame as any as EventBag);
                });

                this.new_frame = true;
            }

        }

    }
    update(game: Game): void {
    }
    get_statue_name(): string {
        return "GameStatue_Start";
    }
}

export { GameStatue_Start };