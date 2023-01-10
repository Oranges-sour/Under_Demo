import { Game } from "./game";

import { EventBag } from "./event";

interface IGameStatue {
    update(game: Game, interval_ms: number): void;
    process_event(game: Game, event: EventBag): void;
    get_statue_name(): string;
}

export { IGameStatue };