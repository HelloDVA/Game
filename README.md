### 五子棋服务器模块

#### 1. 整体设计

GomokuServer 基于独立的 Network 模块（另一个项目），通过组合方式构建，并由智能指针管理生命周期。

其主要功能是接收 HTTP 请求、升级为 WebSocket 连接，并处理五子棋游戏逻辑。

#### 2、网络处理

##### HTTP处理

1. HttpRequest / HttpResponse：封装 HTTP 请求与响应对象，提供统一的接口访问请求行、请求头、请求体，以及构造响应报文。

2. HttpParser：负责解析 HttpRequest，包括请求行、请求头、请求体。解决半包与粘包问题，确保 TCP 流能正确切分为完整 HTTP 报文。

3. HttpProcess：处理基础 HTTP 请求，支持常见的 `GET`、`POST` 方法。

##### WebSocket处理

1. WebSocket 升级，GomokuServer 关闭原有的HTTP连接，将底层 socket 移交给 Boost.Asio提供的 WebSocket 异步 IO 进行处理。

2. 构建IoContext池，来处理具体的Websocket连接，增加系统的并发能力，使用 JSON格式进行消息传递。

#### 3、业务设计

**GameManager（单例）**

1. 负责全局匹配队列，新用户加入时，判断是否能匹配到对手，若满足条件则创建新对局，否则加入等待队列。

2. 管理全局游戏，使用哈希表存储对局（键为全局对局 ID）。利用互斥锁与读写锁控制匹配队列与房间。

3. 解析消息根据，消息类型进行转发，将消息转发至对应的GameSession实例。

##### GameSession

接收游戏消息，处理游戏消息，回复游戏消息。

**Game**

表示一局五子棋对局。负责 落子逻辑、胜负判定，AI功能接口。

































