import { Connection } from "./connection";
import { Tools, Queue } from "./tools";

import { IGameStatue } from "./game_statue";
import { GameStatue_Default } from "./game_statues/default";

import { EventBag } from "./event";

class Game {
    //房主的连接
    _host: Connection;

    _uid: string;

    _players: Map<string, Connection>;

    _event_queue: Queue<EventBag>;

    _last_game: Game;
    _next_game: Game;

    _statue: IGameStatue;

    _is_alive: boolean;

    //游戏房间的描述，如：bob的房间
    _description: string;

    _player_cnt: number;

    constructor(host_connection: Connection, description: string) {
        host_connection.set_game(this);

        this._host = host_connection;
        this._players = new Map<string, Connection>();

        this._uid = Tools.random_string(6);

        this._event_queue = new Queue<EventBag>();

        this._last_game = null!;
        this._next_game = null!;

        this._statue = new GameStatue_Default();

        this._is_alive = true;

        this._description = description;

        this._player_cnt = 1;
    }

    set_is_alive(new_statue: boolean) {
        this._is_alive = new_statue;
    }

    is_alive() {
        return this._is_alive;
    }

    close() {

    }

    get_description() {
        return this._description;
    }

    set_uid(new_id: string) {
        this._uid = new_id;
    }

    get_uid() {
        return this._uid;
    }

    set_last_game(game: Game) {
        this._last_game = game;
    }

    get_last_game() {
        return this._last_game;
    }

    set_next_game(game: Game) {
        this._next_game = game;
    }

    get_next_game() {
        return this._next_game;
    }

    visit_host(call_back: (connection: Connection) => void) {
        call_back(this._host);
    }

    //不包括host
    visit_all_player(call_back: (connection: Connection) => void) {
        this._players.forEach(function (c) {
            call_back(c);
        });
    }

    get_player_cnt() {
        return this._player_cnt;
    }

    add_player(connection: Connection) {
        this._players.set(connection.get_uid(), connection);
        connection.set_game(this);
        this._player_cnt += 1;
    }

    remove_player(connection: Connection) {
        if (this._host != null) {
            if (this._host.get_uid() == connection.get_uid()) {
                this._host = null!;
            }
        }
        this._players.delete(connection.get_uid());

        connection.set_game(null!);
        this._player_cnt -= 1;

        if (this._player_cnt == 0) {
            this.set_is_alive(false);
        }
    }

    swtich_statue(new_statue: IGameStatue) {
        this._statue = new_statue;
    }

    update(interval_ms: number) {
        this._statue.update(this, interval_ms);
        let que = this._event_queue;
        while (!que.empty()) {
            let event = que.front();
            que.pop();

            if (event === undefined) {
                continue;
            }
            this._statue.process_event(this, event);
        }
    }

    push_event(event: EventBag) {
        this._event_queue.push(event);
    }
}

class GamePool {
    _first_game: Game;
    _size: number;

    constructor() {
        this._first_game = null!;
        this._size = 0;
    }

    size() {
        return this._size;
    }

    insert(game: Game) {
        let old_first = this._first_game;

        game.set_next_game(old_first);
        if (old_first !== null) {
            old_first.set_last_game(game);
        }
        this._first_game = game;

        this._size += 1;
    }

    remove(game: Game) {
        let last = game.get_last_game();
        let next = game.get_next_game();

        if (last === null && next === null) {
            this._first_game = null!;
            return;
        }

        //链表头
        if (last === null) {
            this._first_game = next;
            if (next !== null) {
                next.set_last_game(null!);
            }
            return;
        }
        //尾
        if (next === null) {
            last.set_next_game(null!);
            return;
        }

        //中间
        last.set_next_game(next);
        next.set_last_game(last);

        this._size -= 1;
    }

    visit(call_back: (game: Game) => void) {
        let start = this._first_game;
        while (start !== null) {
            call_back(start);
            start = start.get_next_game();
        }
    }
}

let game_pool = new GamePool();

export { Game, game_pool };