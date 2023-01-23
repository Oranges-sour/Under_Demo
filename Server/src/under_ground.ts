import { WebSocketServer } from 'ws';

import { Connection, connection_pool } from './connection';
import { Game, game_pool } from './game';
import { Tools } from './tools';

let socket_server = new WebSocketServer({ port: 23482 });

socket_server.on("connection", function (ws) {
    Tools.log("New connection.");

    let connection = new Connection(ws);
    connection_pool.insert(connection);
});


const UPDATE_INTERVAL_MS = 10;

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