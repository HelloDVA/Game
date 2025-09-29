### 五子棋服务器模块

#### 1. 整体设计

GomokuServer 基于独立的 Network 模块（另一个项目），通过组合方式构建，并由智能指针管理生命周期。

其主要功能是接收 HTTP 请求、升级为 WebSocket 连接，并处理五子棋游戏逻辑。

#### 2. HTTP模块

**HttpRequest / HttpResponse**

封装 HTTP 请求与响应对象，提供统一的接口访问请求行、请求头、请求体，以及构造响应报文。

**HttpParser**

负责解析 HttpRequest，包括请求行、请求头、请求体。

解决半包与粘包问题，确保 TCP 流能正确切分为完整 HTTP 报文。

**HttpProcess**

处理基础 HTTP 请求，支持常见的 `GET`、`POST` 方法。

#### 3. WebSocket模块

当请求为 WebSocket 升级时，GomokuServer 会，关闭原有的 HTTP 连接，将底层 socket 移交给 Boost.Asio提供的 WebSocket 异步 IO 进行处理。

WebSocket 负责承载游戏对战的实时通信，使用 JSON格式进行消息传递，根据 JSON 中的动作类型，调用 GameManager的不同方法。

#### 4. 游戏逻辑模块

**GameManager（单例）**

负责全局匹配队列，新用户加入时，判断是否能匹配到对手，若满足条件则创建新对局，否则加入等待队列。

管理 全局游戏状态，使用哈希表存储对局（键为全局对局 ID），将消息转发至对应的 `Game` 实例。

**Game**

表示一局五子棋对局。负责 落子逻辑、胜负判定。对局过程中的状态更新通过 WebSocket 推送给玩家。





#### Json数据



type : "move"	x : x	y : y	roomid : roomid



































