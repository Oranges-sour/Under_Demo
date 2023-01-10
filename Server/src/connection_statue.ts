import { Connection } from "./connection";

import { Tools, ServerDefault } from "./tools";

import { EventBag } from "./event";

interface IConnectionStatue {
    process_message(connection: Connection, message: string): void;
    process_event(connection: Connection, event: EventBag): void;
    update(connection: Connection, interval_ms: number): void;
}


export { IConnectionStatue };