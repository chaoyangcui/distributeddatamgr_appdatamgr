# distributeddatamgr_appdatamgr

-   [简介](#section11660541593)
-   [目录](#section1464106163817)
-   [约束](#section1718733212019)
-   [软件架构](#section159991817144514)
-   [接口](#section11510542164514)
-   [使用](#section1685211117463)
-   [涉及仓](#section10365113863719)

#### 简介<a name="section11660541593"></a>
分布式数据管理服务，构建分布式数据管理平台，为应用程序和用户提供更加便捷、高效和安全的数据管理能力。降低开发成本，打造应用跨设备运行一致、流畅的用户体验。
当前支持轻量键值（KV）数据，后续会逐步支持其他更丰富的数据类型。
> - 轻量键值（KV）数据：数据有结构，文件轻量，具有事务性，单独提供一套专用的键值对接口
轻量级KV数据库依托当前公共基础库提供的KV存储能力开发，为应用提供键值对参数管理能力。在对有进程的平台上，KV存储提供的参数管理，供单进程访问不能被其他应用使用。在此类平台上，KV存储作为基础库加载在应用进程，以保障不被其他进程访问。

分布式数据管理服务在不同平台上，将数据操作接口形成抽象层用来统一进行文件操作，使厂商不需要关注不同芯片平台文件系统的差异。

#### 目录<a name="section1464106163817"></a>
> 待代码开发完毕后补充

#### 约束<a name="section1718733212019"></a>
由于依赖平台具有正常的文件创建、读写删除修改、锁等能力，针对不同平台尽可能表现接口语义功能的不变，由于平台能力差异数据库能力需要做相应裁剪，其中不同平台内部实现可能不同，具体内容将在详细设计中进行阐述

#### 软件架构<a name="section159991817144514"></a>
##### 轻量键值（KV）数据
KV存储能力继承自公共基础库原始设计，在原有能力基础上进行增强，新增提供数据删除及二进制value读写能力的同时，保证操作的原子性；为区别平台差异，将依赖平台差异的内容单独抽象，由对应产品平台提供。具体设计参考后续详细设计；

#### 接口<a name="section11510542164514"></a>
> 待代码开发完毕后补充

#### 使用<a name="section1685211117463"></a>
> 待代码开发完毕后补充

#### 涉及仓<a name="section10365113863719"></a>
distributeddatamgr_appdatamgr
