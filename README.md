### 1、Network

该项目是Linux系统下基于 epoll 与线程池开发的并发服务器，可以在此基础上实现不同的业务服务器，并配套了常用的工具类库。



### 2、核心模块介绍（主从reactor 线程池）

**channel：**对epoll中——的封装。简化接口并获得更丰富的功能。

**epoller**：对epoll的poll、add、delete、modify操作进行封装。提高复用性与扩展性，并简化接口的使用。

**事件循环**：

##### acceptor

1、创建服务器addr，创建非阻塞socket。

2、利用base loop，bind、listen，服务器socket。

3、创建channel，注册读事件，HandleRead处理，接受连接，调用tcpserver：：回调函数。

4、使用channel，关闭事件，申请从epoll中移除自己。

5、关闭socket。

##### connection

1、利用thread loop，acceptor提供的连接sockfd，创建channel。

2、注册Read，Write，Close，Error事件，并建立对应的处理函数。

3、配备读写缓冲区，读取，发送数据。

4、根据不同事件处理，调用不同的tcpserver：：回调函数。

6、服务器配备主动操作：发送数据，断开连接。



### 3、五子棋服务器模块

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

































### 3、工具模块

日志等工具模块



### 4、测试

#### wrk test

Running 10s test @ http://127.0.0.1:8081
12 threads and 400 connections
Thread Stats   Avg      Stdev     Max   +/- Stdev
   Latency   199.95ms   51.08ms 631.64ms   73.01%
   Req/Sec   163.92     61.62   580.00     67.56%
19595 requests in 10.06s, 1.89MB read
Requests/sec:   1948.68

#### Webbench测试

![1000test](https://github.com/HelloDVA/Network/blob/main/1000test.png)



### 5、主体架构图

![主体架构](https://github.com/HelloDVA/Network/blob/main/network.png)

Library
libboost-all-dev
nlohmann-json3-dev

