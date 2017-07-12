# Server
### Responsibility
- Receive the job from subjob.
- Notify scheduler to do scheduale.
- Send job to mom to execute.

## List
### Server 模組目錄
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


## Models
### 各模組的概略說明
model name | Utilization
------------ | -------------
Server | <ul><li>提供線程池不斷接收任何socket請求</li><li>定時觸發排程要求</li><li>收到通知排程，觸發排程要求</li><li>將收到的請求轉交由Handler處理</li></ul> 
Monitor | <ul><li>JOB queue 的控管</li><li>存取 Node 的資訊</li><li>若 JOB queue 有變更通知 Server 事件</li><li>處理來自 Handler 的各項請求(如:新增工作，排程結束通知，要求 Node 資料等)</li></ul>
Node | <ul><li>存取計算節點的資訊</li></ul>
Handler | <ul><li>處理不同的 request 要求</li><li>必要時會透過 socket 給予請求回應</li></ul>
HandlerFactory | <ul><li>根據請求給予不同種類的 Handler</li></ul>

## Design pattern use
### [Observer](https://sourcemaking.com/design_patterns/observer)
- 用於 Server 與 Monitor 之間
- Server 為 Observer
- Monitor 為 Subject
- 當 Monitor 有變更時，必要時通知 Server
> Observer
```cpp
class Observer
{
	public:
		virtual void notify(int) = 0;       // 當 Subject 有變更時 可透過 notitfy() 通知變更
        virtual void attach_success()=0;    // 當向 Subject 註冊成功後透過 attach_success() 通知
};
```
> Monitor (擷取為 Observer 設計的函式)
```cpp
class Monitor{
    public:
        void attachserver(Observer *obs);   // 註冊 觀察者
    private:
        Observer *observer;
}
```

###[Singleton](https://sourcemaking.com/design_patterns/singleton)
- 用於 Monitor 在多線程中透過GetInstance()的方式 確保取得唯一的 Monitor 物件
> Monitor (擷取為 Singleton 設計的函式)
```cpp
class Monitor{
    public:
        // static method that returns only instance of Moniter
        static Monitor *GetInstance();

    private:
        static Monitor *monitor;
        //holds one and only object of Moniter
        // private constructor
        Monitor(){
            jobtex.unlock();
        };
}
```

###[Strategy](https://sourcemaking.com/design_patterns/strategy)
- 用於 Handler ，Handler為處理不同request的物件，透過 Strategy 方式，能使所有 Handler 之間有共同介面能呼叫。
> IHandler
```cpp
class IHandler
{
    public:
    virtual void handle() = 0;  // 透過共同介面 IHandler ，所有 Handler 皆繼承此介面
                                // 僅須透過 handle() 即可呼叫
}; 
```

###[Factory](https://sourcemaking.com/design_patterns/factory_method)
- 用於 HandlerFactory，能因應不同的 request 產生應對的 Handler
>> HandlerFactory.hpp
```cpp
class HandlerFactory{
    public:
    static IHandler *getHandler(json,s_socket*);
};
```

## <a name = "main"></a> main
```cpp
int main(int argc, char **argv){
    /*
        偵錯模式選擇
        -fdebug --> 檔案模式偵錯
        -odebug --> 輸出模式偵錯
    */
    if(argc > 1){
        string arg;
        arg = std::string(argv[1]);
        if(arg == "-fdebug"){
            debug = 1;
            debug_file = new ofstream();
            debug_file->open("debug.txt");
        }
        else if(arg == "-odebug"){
            debug = 2;
        }
    }

    /*
        讀入 server 設定檔 設定本 server 的 ip,port
    */
    ifstream f("server.con");
    string server_ip,server_port;
    f >> server_ip;
    f >> server_port;
    f.close();

    /*
        讀入 scheduler 設定檔 告知 server scheduler的位子
    */
    f.open("scheduler.con");
    string scheduler_ip,scheduler_port;
    f >> scheduler_ip;
    f >> scheduler_port;
    f.close();

    /*
        fork() 使子程序成為背景程序
    */
    pid_t pid= fork();
	if(pid == -1) {
    	cout << "server fork error !" << endl;
        exit(EXIT_FAILURE);
    }
    else if(pid != 0) {
        if(debug > 0){
            if(debug == 1)
                *debug_file << "shut down parent !" << endl;
            else if(debug == 2)
                cout << "shut down parent !" << endl;
        }
    	return 0;
    }

    Monitor::GetInstance()->setnodelist();                  // 設定 Monitor 的 Node list
    Server server(Monitor::GetInstance());                  // 創立 Server 物件 並向 Monitor 註冊為觀察者 (觀察者模式)
                                                            // 當 Monitor 有變更 有必要時會通知 Server
    server.set_server_attr(server_ip,server_port);          // 設定 Server 的 Ip 及 Port
    server.set_scheduler_attr(scheduler_ip,scheduler_port); // 設定 Server 要求的 Scheduler 位子
    server.run();                                           // 開始執行 Server

}
```