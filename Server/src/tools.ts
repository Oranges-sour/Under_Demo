class Tools {
    static parse_JSON_safe(str: string): Object | null {
        try {
            return JSON.parse(str);
        } catch (e) {
            return null;
        }
    }

    static random_string(length: number) {
        const ch = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
            'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
            'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'];

        let str = "";
        for (let i = 1; i <= length; ++i) {
            let r = Math.round(Math.random() * (ch.length - 1));
            str += ch[r];
        }

        return str;
    }

    static append_zero(t: number, digit: number) {
        let max_t = Math.pow(10, digit - 1);

        let result = "";

        while (max_t > 1) {
            if (t < max_t) {
                result += "0";
            } else {
                break;
            }
            max_t /= 10;
            max_t = Math.floor(max_t);
        }

        return `${result}${t}`;
    }

    static encode(str: string) {
        return str;
    }

    static decode(str: string) {
        return str;
    }

    static log(message: string) {

        let apz = Tools.append_zero;

        let date = new Date();
        let month = apz(date.getMonth() + 1, 2);
        let day = apz(date.getDate(), 2);
        let hour = apz(date.getHours(), 2);
        let minute = apz(date.getMinutes(), 2);
        let second = apz(date.getSeconds(), 2);
        let mili_sec = apz(date.getMilliseconds(), 3);

        console.log(`${month}/${day} ${hour}:${minute}:${second}:${mili_sec} >> ${message}`);
    }

}

//服务器的基本信息
class ServerDefault {
    //获得服务器的版本信息
    static get_server_version() {
        return "v0.0.1";
    }

    //获得服务器对应的客户端版本
    static get_required_client_version() {
        return "v0.0.1";
    }



}

interface IQueue<T> {
    push(item: T): void;
    front(): T | undefined;
    pop(): void;
    size(): number;
    empty(): boolean;
}

class Queue<T> implements IQueue<T> {
    private storage: T[] = [];

    constructor(private capacity: number = Infinity) { }

    push(item: T): void {
        if (this.size() === this.capacity) {
            throw Error("Queue has reached max capacity, you cannot add more items");
        }
        this.storage.push(item);
    }
    front(): T | undefined {
        return this.storage.at(0);
    }
    pop(): void {
        this.storage.shift();
    }
    size(): number {
        return this.storage.length;
    }
    empty(): boolean {
        return (this.storage.length == 0);
    }
}

export { Tools, ServerDefault, Queue };