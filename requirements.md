# cpp_network_learning 需求文档

## 1. 项目目标

本项目实现一个用于学习和验证 C++ 网络编程模型的 HTTP/TCP 服务器。项目重点展示 Linux 下基于 epoll 的 Reactor 模型、非阻塞 socket、连接生命周期管理、HTTP 请求解析、响应生成、日志输出和基础压测能力。

## 2. 运行环境需求

### 2.1 基础环境

- 操作系统：Linux 或 WSL2。
- 编译器：支持 C++17 的 g++。
- 构建工具：make。
- 线程库：pthread。
- 网络能力：支持 epoll、timerfd、非阻塞 socket。

### 2.2 推荐开发环境

- Ubuntu 22.04 或兼容 Linux 发行版。
- g++ 11 或更高版本。
- make 4.x。
- curl 用于手动访问 HTTP 服务。
- apache2-utils 中的 ab 可用于基础压测。

### 2.3 Agent 沙箱环境

Agent 执行任务时应优先使用仓库根目录下的 `.agent-sandbox.yml`。该配置会使用 Ubuntu 22.04 镜像，并安装编译本项目所需的最小依赖。

沙箱验收命令：

~~~bash
make clean && make
~~~

## 3. 功能需求

### 3.1 HTTP 服务

- 系统应能编译生成 `server` 可执行文件。
- `server` 启动后应监听本地 HTTP 服务端口。
- HTTP 服务应支持基础 GET 请求处理。
- 对已定义路径应返回对应响应内容。
- 对未定义路径应返回 404 响应。
- 对无法解析或不支持的请求应返回错误响应并安全关闭连接。

### 3.2 Reactor 网络模型

- 系统应使用 main Reactor 负责监听和接收新连接。
- 系统应使用 sub Reactor 负责已建立连接的读写事件处理。
- 每个连接应通过 Channel 注册读写事件和回调。
- EventLoop 应通过 epoll 获取活跃事件并分发到 Channel。
- 连接关闭时应延迟清理 Channel，避免事件回调期间释放正在使用的对象。

### 3.3 连接与缓冲区

- 每个连接应维护独立的读缓冲区和写缓冲区。
- 读取逻辑应支持非阻塞读取，直到遇到 EAGAIN/EWOULDBLOCK、对端关闭或错误。
- 发送逻辑应支持部分写入。
- 当写入未完成时，应注册写事件并在后续 EPOLLOUT 中继续发送。
- 当响应要求关闭连接时，应在发送完成后关闭连接。

### 3.4 HTTP 解析

- HTTP 解析器应维护请求解析状态。
- 解析结果应包含 method、url、query params、protocol、version、headers 和 body。
- 解析失败时应进入错误处理分支，不应导致进程崩溃。
- 修改解析器时应保留当前代码中使用的 `ParaseRequest` 接口名，除非同时完成全局重命名和验证。

### 3.5 日志

- 系统应支持运行时开启或关闭 debug 日志。
- `server` 应支持 `--log`、`-l`、`--log=1`、`--no-log`、`--log=0` 等参数。
- 默认运行不应输出过量调试信息。

### 3.6 压测与人工验证

- 项目应支持使用 curl 进行基本 HTTP 请求验证。
- 项目可使用 ab 进行基础并发压测。
- 压测参数应根据本机或 WSL 环境能力调整，避免过高并发导致宿主环境异常。

## 4. 非功能需求

### 4.1 可维护性

- 新增代码应保持当前 C++17 风格。
- 优先使用现有模块边界，不应随意引入大型框架。
- 修改连接生命周期、EventLoop 或 Channel 时，应说明对象所有权和释放时机。

### 4.2 稳定性

- 网络错误、对端关闭、解析失败等情况不应导致进程崩溃。
- 连接删除应避免 use-after-free。
- 非阻塞读写应正确处理 EAGAIN/EWOULDBLOCK。

### 4.3 可验证性

- 每次修改后至少应运行：

~~~bash
make clean && make
~~~

- 涉及 HTTP 行为时，应启动 `server` 后使用 curl 验证关键路径。
- 涉及并发或连接生命周期时，应补充压力测试或重复连接测试。

## 5. 构建与运行

### 5.1 构建

~~~bash
make clean && make
~~~

### 5.2 启动服务

~~~bash
./server
~~~

开启日志：

~~~bash
./server --log
~~~

### 5.3 手动验证

~~~bash
curl -i http://127.0.0.1:9999/
curl -i http://127.0.0.1:9999/hello
curl -i http://127.0.0.1:9999/not-found
~~~

### 5.4 压测示例

~~~bash
ab -k -n 2000 -c 100 http://127.0.0.1:9999/
~~~

并发数应根据本地环境调整。

## 6. Agent 执行要求

- Agent 处理 Issue 前应先阅读相关源码和当前 Issue 描述。
- 涉及构建、测试、压测命令时，应优先在 Docker 沙箱中执行。
- 如果 Docker 不可用并切换到本地执行，Agent 应明确说明本地环境依赖要求。
- Agent 修改代码后应运行最小验证命令 `make clean && make`。
- 如果验证失败，应在最终回复中说明失败命令、错误原因和未完成事项。

## 7. 当前已知风险

- HTTP 解析状态机对分片请求和异常格式较敏感。
- `HttpRequest::GetRequestValue` 的行为需要结合实际代码确认，避免误读 query 参数和 headers。
- `TimeStamp::ToFormattedString()` 当前实现需要检查，避免声明和行为不一致。
- 高并发压测可能受 WSL、文件描述符限制和本机网络栈限制影响。
