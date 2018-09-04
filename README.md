# NGX

一个以NGINX为原型修改的高性能服务端框架

## 最终目标

实现一个：

-   高效率
-   稳定性好
-   易于扩充
-   编码风格良好

的服务端框架。

## 实现情况


-   基本的数据结构
-   Pool和PageAlign的内存分配器

-   相同用途Pool的回收和预留，减少调用`malloc()`和`free()`的次数。
    
    *  PoolRecycler
        在销毁连接时根据运行状况保留一部分Pool, 在新连接创建时直接回收使用。

-   基于红黑树的高级数据结构

    *   FSTree 【基本完成】
        使用哈希和红黑树，组织多层的文件系统资源索引树。
        首先使用哈希匹配，然后使用具体匹配，实现`log(m)+n`复杂度的资源索引(`m`为树的基数，n为字符串长度)。
        这个模块主要用途 
        _在HTTP解析完报文并且经过PreponeFilte之后根据URI路由请求。_
        -----------------------------------------------------
        同时使用Hash和字符串比较会出错，哈希和字符串没有大小映射关系。
        使用矩母函数效率低下，大约有11%的查询会被简化。
        后续使用分层Hash来解决这个问题。

    *   TimerTree [完成]
        用在定时事件的组织和处理。
    
    *   DNSResolver
        Resolver中解析地址索引和地址的Expire处理。

    *   FileCache
        用于根据 `(FileNameHash, FileName, Timestamp, FileContentSHA1)`进行数据缓存。
        用于实现基于 `Version-Based`热点流数据缓存。定期用Promise来对`Expire`的数据切片进行清理。

-   基于`Promise`的线程池 【已完成】

    基于`Promise设计模式`的`then`和`error`来定义请求的处理流程的`DAG`。`Promise`将请求处理切分成小块，在一个过程结束之后将下一个过程存入队列中，在保证逻辑上的顺序之后允许程序以一种灵活的方式在线程池中流转。
    这个方法将带来两个弊端：
    *   多分段切分可能会降低程序的顺序执行程度（影响流水线）
    *   对应`EPoll`模型使用中，为防止主线程拉取到新的数据发出新的`Promise`应当将对读写事件的响应作为`Promise`形成的`DAG`的尾端处理。这样会可能会增大单个请求的延迟。
    所以对于*简单逻辑*或者*低延迟要求*的任务尽量直接在主线中完成。

    整体感觉是略微（或一定程度上）削弱单个请求（或连接）的响应。但增加整体的流通率。

-   Connection & Listen 类 【正在实现】

    -   基本的Socket连接操作
    -   基本的服务端Socket监听
    -   Resolver和Resolver树的维护，HTTPDNS扩展

-   锁
    -   Atomic
        用在细粒度的资源保护，比如ThreadPool中的PromiseQueue和Time模块中的时间戳
    -   FileLock
        文件锁，适应多进程（多副本）服务端运行。
        解决多进程的`惊群`问题
    -   实现 Relax-Machine 操作
        `__asm__ __volatile__("rep;nop": : :"memory");`

-   Time(Provider)【已完成】

    监听定时中断，刷新时间戳。
    设置Resolution,实现懒更新时间。

- EventDomain 
    分域的事件处理器，首先实现EpollEventDomain
    通过组装TimerTree, EPollEvent, ThreadPool 实现完备的事件处理器

- I\O 
    常规的TCP和UDP send recv, writeiov, readiov, sendfile, aio等IO方式，并根据动态检测来配置使用；
- HTTP Server
  通过组装EventDomain和FSTree实现基本的连接接管和资源控制，再实现HTTP的一组Handle来处理HTTP服务；
  扩充，装配PoolRecycler 循环使用内存。

### 构想中的部分

- 通过File Lock 和 yaml 实现多进程副本的协同工作
  
### 需要解决但是没思路的问题

    多进程的FileSystemCache共享, 需要使用共享内存。
    如何组织Relocatable的FileCache?
    事实上除了这一点多个进程之间的联系仅是通过几个锁同步
    (建立在地址端口使用REUSE_PORT实现复用)。
### 扩展思考
并行状态机和计算Offload；
在上述实现后，Address主要的性能开销，通过Offload到硬件来进一步加速服务性能；
1. 将Hash检索通过硬件加速，批量写入、分页计算；
2. 通过协处理的方式将Hash和字符串比较的功能集成到RISC V等处理器上；
3. 在X86上对长度小于8字节的字符串用 XMM 等指令集进行比较；

