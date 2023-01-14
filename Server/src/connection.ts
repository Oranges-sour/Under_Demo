import { WebSocket } from 'ws';

import { Tools, Queue } from './tools';

import { Game } from './game';

import { IConnectionStatue } from './connection_statue';
import { ConnectionStatue_Default } from './connection_statues/default'

import { EventBag } from './event';

class Connection {
    _socket: WebSocket;
    _last_connection: Connection;
    _next_connection: Connection;
    _is_alive: boolean;

    _is_on_close: boolean;

    _statue: IConnectionStatue;

    _uid: string;

    _game: Game;

    constructor(web_socket: WebSocket) {

        this._socket = web_socket;

        this._last_connection = null!;
        this._next_connection = null!;

        this._is_alive = true;

        this._is_on_close = false;

        this._game = null!;

        this._uid = "";

        this._statue = new ConnectionStatue_Default();

        let that = this;
        this._socket.on("message", function (e) {
            that.process_message(Tools.decode(e.toString()));
        });

        this._socket.on("close", function () {
            let event = new EventBag();
            event.type = "connection_on_close";
            that.push_event(event);

        });

        this._socket.on("error", function () {
            let event = new EventBag();
            event.type = "connection_on_close";
            that.push_event(event);
        });
    }

    set_is_on_close(new_statue: boolean) {
        this._is_on_close = new_statue;
    }

    get_is_on_close() {
        return this._is_on_close;
    }

    set_uid(new_uid: string) {
        this._uid = new_uid;
    }

    get_uid() {
        return this._uid;
    }

    set_is_alive(new_statue: boolean) {
        this._is_alive = new_statue;
    }

    is_alive() {
        return this._is_alive;
    }

    close() {
        this._socket.close();
    }

    socket() {
        return this._socket;
    }

    get_game() {
        return this._game;
    }

    set_game(game: Game) {
        this._game = game;
    }

    remove_from_game() {
        if (this._game !== null) {

            let event = new EventBag();
            event.type = "quit_game";
            event.info_1 = this.get_uid();
            this._game.push_event(event);
        }
    }

    set_last_connection(connection: Connection) {
        this._last_connection = connection;
    }

    get_last_connection() {
        return this._last_connection;
    }

    set_next_connection(connection: Connection) {
        this._next_connection = connection;
    }

    get_next_connection() {
        return this._next_connection;
    }

    send_message(obj: Object, call_back?: (err?: Error) => void) {
        if (!this.is_alive() || this.get_is_on_close()) {
            return;
        }
        let str = JSON.stringify(obj);
        this._socket.send(Tools.encode(str), call_back);
    }

    switch_statue(new_statue: IConnectionStatue) {
        this._statue = new_statue;
    }

    process_message(message: string) {
        this._statue.process_message(this, message);
    }

    update(interval_ms: number) {
        this._statue.update(this, interval_ms);
    }

    push_event(event: EventBag) {
        this._statue.process_event(this, event);
    }
}

class ConnectionPool {
    _first_connection: Connection;
    _size: number;

    constructor() {
        this._first_connection = null!;
        this._size = 0;
    }

    size() {
        return this._size;
    }

    insert(connection: Connection) {
        let old_first = this._first_connection;

        connection.set_next_connection(old_first);
        if (old_first !== null) {
            old_first.set_last_connection(connection);
        }
        this._first_connection = connection;

        this._size += 1;
    }

    remove(connection: Connection) {
        let last = connection.get_last_connection();
        let next = connection.get_next_connection();

        //链表只有一个元素
        if (last === null && next === null) {
            this._first_connection = null!;
            return;
        }

        //链表头
        if (last === null) {
            this._first_connection = next;
            if (next !== null) {
                next.set_last_connection(null!);
            }
            return;
        }

        //链表尾
        if (next === null) {
            last.set_next_connection(null!);
            return;
        }

        //中间
        last.set_next_connection(next);
        next.set_last_connection(last);

        this._size -= 1;
    }

    visit(call_back: (connection: Connection) => void) {
        let start = this._first_connection;
        while (start !== null) {
            call_back(start);
            start = start.get_next_connection();
        }
    }

}

let connection_pool = new ConnectionPool();

export { Connection, connection_pool };