# Server
### Responsibility
- Receive the job from subjob.
- Notify scheduler to do scheduale.
- Send job to mom to execute.

## List
- [main](#main)
- [Server](#Server)
- [Monitor](#Monitor)
- [Node](#Node)
- [Handler](#Handler)
- [HandlerFactory](#HandlerFactory)
- Library Dependency
    - [Message](./library.md#Message)
    - [c_socket](./library.md#ccsocket)
    - [s_socket](./library.md#ssocket)
    - [ThreadPool](./library.md#ThreadPool)


### Models

model name | utilization
------------ | -------------
Server | 
1. 提供線程池不斷接收任何socket請求 
2. 定時觸發排程要求 
3. 收到通知排程，觸發排程要求 
4. 將收到的請求轉交由Handler處理
Monitor | 
1. JOB queue 的控管
2. 存取 Node的資訊
3. 若 JOB queue 有變更通知 Server 事件
4. 處理來自 Handler 的各項請求(如:新增工作，排程結束通知，要求 Node 資料等)
Node | 
1. 存取計算節點的資訊
Handler | 
1. 處理不同的 request 要求
2. 必要時會透過 socket 給予請求回應
HandlerFactory |
1. 根據請求給予不同種類的 Handler
