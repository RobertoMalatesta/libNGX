# NGX

一个以 nginx 为原型修改的高性能服务端基础设施库

### 基本组件

1.  ~~页对齐块内存分配器(MemoryBlockAllocator和BufferMemoryBlock)~~
2.  ~~内存池(Pool)~~
3.  ~~块内存回收器(Recycle)~~
4.  ~~轻量级纤程池(ThreadPool) [部分完成]~~
5.  ~~使用Hash作为Key的红黑树(Dictionary),通过前后查找解决冲突~~
6.  ~~基于红黑树的定时器(TimerHub)~~
7.  ~~数据缓冲(Buffer)~~
8.  ~~网络通信方面的类(Connection, Listening, EventDomain)~~
9.  ~~常用的校验、摘要、哈希算法~~
10. DNS解析器
11. FileCache
12. 连接池，用来实现Upstream功能(ConnectionPool)

### HTTP模块

1.  实现了~~HTTPRequest, HTTPResponse, HTTP Header~~, HTTP Chunk的解析。
2.  HTTP转发框架的设计和实现，剥离转发和具体的服务功能。
3.  支持转发模块的热更新，服务的动态配置。
4.  服务：HTTP静态资源分发、流量转发、镜像、代理、限流、分流、熔断、性能监控[未实现]

## 存在问题

1.  多线程模型和多进程模型的选择和组合。
2.  多进程文件缓存的实现。

## 构想

1.  借助 FPGA 和 CUDA实现并行的 HTTP 报文解析。
2.  实现HTTP2.0和grpc等高效通信支持。
