class EventBag {
    type: string;
    info_1: string;
    info_2: string;
    info_3: string;
    info_4: Array<string>;

    constructor() {
        this.type = "";
        this.info_1 = "";
        this.info_2 = "";
        this.info_3 = "";
        this.info_4 = new Array<string>;
    }
}

export { EventBag };