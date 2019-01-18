# NGX

一个以 nginx 为原型修改的高性能服务端基础设施库

### 基本组件

1.  页对齐块内存分配器(MemoryBlockAllocator和BufferMemoryBlock) [完成]
2.  内存池(Pool) [完成]
3.  块内存回收器(Recycle) [完成]
4.  轻量级纤程池(ThreadPool) [部分完成]

    - Thread分组，以实现更高的代码局部性（对线程编码，通过汉明距离来表示处理代价）。

    - Task Steal 允许空闲线程协助接管任务。

5.  使用Hash作为Key的红黑树(Dictionary),通过前后查找解决冲突 [完成]
6.  基于红黑树的定时器(TimerHub) [完成]
7.  数据缓冲(Buffer) [完成]
8.  网络通信方面的类(Connection, Listening, EventDomain) [完成]
9.  常用的校验、摘要、哈希算法 [完成]
10. DNS 解析器
11. FileCache
12. 连接池，用来实现Upstream功能(ConnectionPool)

### HTTP模块

1.  实现了基本的HTTP协议处理框架，正在完善Header的解析后处理。
2.  HTTP服务框架，正在设计可热插拔，易于集群管理的服务框架。
3.  HTTP限流，性能监控 [未实现]
4.  HTTP流量转发，镜像 [未实现]
5.  HTTP 静态资源缓存 [未实现]

## 存在问题

1.  多线程模型和多进程模型的选择和组合。
2.  多进程文件缓存的实现。

