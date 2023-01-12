import { WebSocketServer } from 'ws';

import { Connection, connection_pool } from './connection';
import { Game, game_pool } from './game';
import { Tools } from './tools';

let socket_server = new WebSocketServer({ port: 23483 });

socket_server.on("connection", function (ws) {
    Tools.log("New connection.");

    let connection = new Connection(ws);
    connection_pool.insert(connection);
});


const UPDATE_INTERVAL_MS = 16;

setInterval(function () {
    main_update(UPDATE_INTERVAL_MS);
}, UPDATE_INTERVAL_MS);

function main_update(interval_ms: number) {
    update_connections(interval_ms);
    update_games(interval_ms);
}


function update_connections(interval_ms: number) {
    let need_to_remove = new Array<Connection>();

    function on_visit(connection: Connection) {
        connection.update(interval_ms);

        if (!connection.is_alive()) {
            connection.close();
            need_to_remove.push(connection);
        }
    }
    connection_pool.visit(on_visit);

    need_to_remove.forEach(function (connection: Connection) {
        connection_pool.remove(connection);
    });

    if (need_to_remove.length != 0) {
        Tools.log(`Removed ${need_to_remove.length} connection.`);
    }
}

function update_games(interval_ms: number) {
    let need_to_remove = new Array<Game>();

    function on_visit(game: Game) {
        game.update(interval_ms);

        if (!game.is_alive()) {
            game.close();
            need_to_remove.push(game);
        }
    }
    game_pool.visit(on_visit);

    let uids = "";

    need_to_remove.forEach(function (game: Game) {
        uids += ` ,${game.get_uid()}`;

        game_pool.remove(game);
    });

    if (need_to_remove.length != 0) {
        Tools.log(`Removed ${need_to_remove.length} game. uid: ${uids}`);
    }
}



class aa {
    type: string;
    param1: number;
    constructor() {
        this.type = "123";
        this.param1 = 123;
    }
}


class bb {
    type: string;
    constructor(){
        this.type = "";
    }
}


let a = new aa();
let obj = a as Object;
let b = obj as bb;
let a1 = b as aa;

console.log(a);
console.log(b);
console.log(a1);