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
Server | <ul><li>提供線程池不斷接收任何socket請求</li><li>定時觸發排程要求</li><li>收到通知排程，觸發排程要求</li><li>將收到的請求轉交由Handler處理</li></ul> 
Monitor | <ul><li>JOB queue 的控管</li><li>存取 Node 的資訊</li><li>若 JOB queue 有變更通知 Server 事件</li><li>處理來自 Handler 的各項請求(如:新增工作，排程結束通知，要求 Node 資料等)</li></ul>
Node | <ul><li>存取計算節點的資訊</li></ul>
Handler | <ul><li>處理不同的 request 要求</li><li>必要時會透過 socket 給予請求回應</li></ul>
HandlerFactory | <ul><li>根據請求給予不同種類的 Handler</li></ul>
