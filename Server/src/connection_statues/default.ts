import { IConnectionStatue } from "../connection_statue";
import { Connection } from "../connection";
import { Tools } from "../tools";
import { ServerDefault } from "../tools";

import { ConnectionStatue_Normal } from "./normal";
import { EventBag } from "../event";

/*刚连接时的状态，现在还不确定是否为正常连接
客户端要发送验证消息，否则连接将被断开
*/
class ConnectionStatue_Default implements IConnectionStatue {
    time_ms: number;
    constructor() {
        this.time_ms = 0;
    }
    process_message(connection: Connection, message: string): void {
        const COMING_MSG_TYPE = "client_check";
        const SENDING_MSG_TYPE = "client_check_result";
        const SENDING_STATUE_SUCCESS = "success";
        const SENDING_STATUE_FAILED = "failed";
        const ERR_INFO = {
            bad_version: "bad_version",
            bad_security_key: "bad_security_key"
        }
        const SECURITY_KEY = "1kd73b0pmz37ru91saqkm0286ti61ckg";

        let obj = Tools.parse_JSON_safe(message);
        // 消息不对，断开连接
        if (obj === null) {
            connection.set_is_alive(false);
            return;
        }

        const MSG = {
            type: "",
            version: "",
            uid: "",
            security_key: "",
        };

        let msg = obj as typeof MSG;
        if (msg.type === undefined ||
            msg.version === undefined ||
            msg.uid === undefined ||
            msg.security_key === undefined) {
            connection.set_is_alive(false);
            return;
        }

        if (msg.type != COMING_MSG_TYPE) {
            return;
        }


        //版本不对应，返回错误，并断开连接
        if (msg.version != ServerDefault.get_required_client_version()) {
            const bad_version = {
                type: SENDING_MSG_TYPE,
                statue: SENDING_STATUE_FAILED,
                info: ERR_INFO.bad_version,
            }
            connection.send_message(bad_version, function (err) {
                if (err !== undefined) {
                    Tools.log(`ConnectionStatue_Default: >bad_version< send_failed: ${err.message}`);
                    return;
                }
                connection.set_is_alive(false);
            });
        }

        //密钥不对应
        if (msg.security_key != SECURITY_KEY) {
            const bad_security_key = {
                type: SENDING_MSG_TYPE,
                statue: SENDING_STATUE_FAILED,
                info: ERR_INFO.bad_security_key,
            }
            connection.send_message(bad_security_key, function (err) {
                if (err !== undefined) {
                    Tools.log(`ConnectionStatue_Default: >bad_security_key< send_failed: ${err.message}`);
                    return;
                }
                connection.set_is_alive(false);
            });
        }

        //检查完毕，是正常连接
        const success = {
            type: SENDING_MSG_TYPE,
            statue: SENDING_STATUE_SUCCESS,
            info: "",
        }
        connection.send_message(success);

        connection.set_uid(msg.uid);

        //切换状态状态机
        connection.switch_statue(new ConnectionStatue_Normal());
    }
    process_event(connection: Connection, event: EventBag): void {
        if (event.type == "connection_on_close") {
            connection.set_is_on_close(true);
            connection.set_is_alive(false);
        }

    }
    update(connection: Connection, interval_ms: number): void {
        this.time_ms += interval_ms;
        //三秒还没完成验证，直接关闭
        if (this.time_ms >= 3000) {
            connection.set_is_alive(false);
        }
    }
}

export { ConnectionStatue_Default };