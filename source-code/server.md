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

### [Singleton](https://sourcemaking.com/design_patterns/singleton)
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

### [Strategy](https://sourcemaking.com/design_patterns/strategy)
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

### [Factory](https://sourcemaking.com/design_patterns/factory_method)
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

## <a name = "Server"></a> Server
1. `Server.hpp` 概觀
```cpp
class Server : public Observer{
    public:
        // 建構子，輸入要註冊的Subject
        Server(Monitor *m);
        // 設定屬性 svr_ip及svr_port
        void set_server_attr(std::string,std::string);
        // 設定屬性 sch_ip及sch_port
        void set_scheduler_attr(std::string,std::string);
        // 執行
        void run();
        // Subject 透過 notify 通知變更
        void notify(int);
        // Subject 透過 attach_success 通知註冊成功
        void attach_success();
    private:
        // 向 scheduler 通知排程
        int contact_scheduler();
        // 定時檢測是否要觸發排程
        void check_schedule();
        // 開啟 accept_thread 接收 socket 的 connect 要求
        static void start_accept_thread(std::string,std::string,ThreadPool*);
        // 讀取接收的要求 並轉交 Handler 處理
        static void readrequest(s_socket*,HandlerFactory*);
        // 屬性 接收要求的線程池
        ThreadPool *request_pool;
        // 屬性 Server 的線程池
        ThreadPool *server_pool;
        // 屬性 布林直 使否要觸發排程要求
        bool do_schedual = false;
        // 屬性 布林直 排程器是否忙碌中
        bool schedual_busy = false;
        // 屬性 定期要多久觸發依次排程 EX: 60s
        int sched_iteration;
        // 屬性 時間 下次排程時間，現在時間
        time_t next_schedule,now_time;
        // 屬性 mutex鎖 保護 布林直被多線程同時更動的可能
        std::mutex do_schedual_tex,schedual_busy_tex;
        // 屬性 server 及 scheduler 的 IP 和 PORT
        std::string svr_ip,svr_port,sch_ip,sch_port;
};
```
2. `建構子`
```cpp
Server::Server(Monitor *m){
    m->attachserver(this);  // 在 Monitor 中註冊本 Server 物件
}
```
3. `set_XXX_attr()` 設定 Server 屬性
> set_server_attr(): 設定 Server 的 IP 和 PORT
```cpp
void Server::set_server_attr(string ip, string port){
    svr_ip = ip;
    svr_port = port;
}
```
> set_scheduler_attr(): 設定要求的排程器的 IP 和 PORT
```cpp
void Server::set_scheduler_attr(string ip, string port){
    sch_ip = ip;
    sch_port = port;
}
```

4. `notify()` 當 Monitor 有需要通知變更時，即透過 notify() 進行通知
```cpp
void Server::notify(int msg){
    // msg == 0 notitfy newjob 新工作通知
    // msg == 1 notitfy schedual finish 排程器結束排程通知
    if(msg == 0){
        if(debug > 0){
        do_schedual_tex.lock();
        do_schedual = true;
        do_schedual_tex.unlock();
    }
    else if(msg == 1){
        schedual_busy_tex.lock();
        schedual_busy = false;
        schedual_busy_tex.unlock();
    }
}
```

5. 核心函式 `run()`
```cpp
void Server::run(){
    /*
        step 1:開一條線程 進行 start_accept_thread() 的工作
    */
    server_pool = new ThreadPool(1);
    server_pool->enqueue(start_accept_thread,svr_ip,svr_port,request_pool);
    /*
        step 2:設定 定時要求排程的延遲
    */
    sched_iteration = 60; //1 min
    now_time = time(NULL);
    next_schedule = now_time + sched_iteration;
    int count =0;   // debug 用 用來記錄要求了多少次排程
    /*
        step 3: 開使跑 while() 迴圈 不斷檢視是否要要求排程
    */
    while(1){
        /*
            step 1:檢測是否超過定時要求的排程時間
        */
        check_schedule();
        do_schedual_tex.lock();
        /*
            step 2:檢測布林直 do_schedual 是否要進行排程要求
        */
        if(do_schedual){
            /*
                step 3: 如果要進行排程 先間定時排程時間往後推移
            */
            next_schedule = now_time + sched_iteration;
            /*
                step 4: 檢視布林直 schedual_busy 查看排程器是否忙碌中
            */
            if(!schedual_busy){
                /*
                    step 4 (不忙碌): 向排程器通知要求排程
                */
                if(contact_scheduler() == 1){
                    do_schedual = false;
                    do_schedual_tex.unlock();
                    schedual_busy_tex.lock();
                    schedual_busy = true;
                    schedual_busy_tex.unlock();
                }
                else{
                    /*
                        如果通知失敗 取消布林直 do_schedual 等待下次定時排程要求
                    */
                    do_schedual = false;
                    do_schedual_tex.unlock();
                }
            }
            else{
                /*
                    step 4 (忙碌): 取消布林直 do_schedual 等待下次定時排程要求
                */
                do_schedual = false;
                do_schedual_tex.unlock();
            }
        }
    }
}
```

6. `check_schedule()`:檢測是否超過定時要進行排程要求的時間
```cpp
void Server::check_schedule(){
    /*
        step 1: 更新當前時間
    */
    now_time = time(NULL);
    /*
        step 2: 檢視下次排程時間 next_schedule 是否已經到達
    */
    if(next_schedule - now_time <= 0){
        /*
            如果已經到達或超時 更改 布林直 do_schedual 設為 true
        */
        do_schedual_tex.lock();
        do_schedual = true;
        do_schedual_tex.unlock();
    }
}
```

7. `contact_scheduler()`:向排程器通知要求排程
```cpp
int Server::contact_scheduler(){
    /*
        step 1: 創立一 cliet端 socket c_socket
        step 2: setConnection() 設定要連線到的排程器
        step 3: connect2server() 向排程器要求連線
        失敗回傳 0
    */
    c_socket s;
    Message do_schedule_msg;
    if(s.setConnection(sch_ip,sch_port) != 1){
        return 0;
    }
    if(s.connect2server() != 1){
        return 0;
    }
    /*
        step 4: 封裝要給排程器的訊息
    */
    do_schedule_msg.initMessage();
    do_schedule_msg.msg["IP"] = svr_ip;
    do_schedule_msg.msg["PORT"] = svr_port;
    do_schedule_msg.encode_Header("server","scheduler","do_schedule");
    /*
        訊息封裝結果:
        {
            "IP" : "127.0.0.1"
            "Port" : "5001"
            "SENDER" : "server"
            "RECEIVER" : "scheduler"
            "REQUEST" : "do_schedule"
        }
    */
    /*
        step 5: 傳送訊息
        成功回傳 1
    */
    s.send(do_schedule_msg.encode_Message());
    s.closeConnection();
    return 1;
}
```

8. `start_accept_thread()` : 開啟 server socket 接收到連線後 立刻將 socket 封裝到線程池 並開啟下一個socket接收連線
```cpp
void Server::start_accept_thread(string ip, string port, ThreadPool* pool){
    /*
        開啟 10個線程
    */
    pool = new ThreadPool(10);
    /*
        創立一個 HandlerFactory
    */
    HandlerFactory *factory = new HandlerFactory();
    /*
        核心迴圈
    */
    while(1){
        /*
            step 1: 開啟一 server端socket物件 s_socket
            step 2: setConnection() 設定 server socket 
            step 3: acceptClinet() 等待 client socket的連線
        */
        s_socket *s = new s_socket();
        if(s->setConnection(ip,port) != 1){
            continue;
        }
        if(s->acceptClinet()){
            /*
                接收連線後
                step 1: 設定 accept socket fd能重用
                step 2: 解除該server socket的綁定
                step 3: 將該 socket 和 HandlerFactory 封裝入線程池 透過 readrequest解讀要求
            */
            s->setacceptreuse();
            s->closebind();
            pool->enqueue(&readrequest,s,factory);
        }
    }
}
```

9. `readrequest()`:透過核心迴圈讀取要求，最後結束連線
```cpp
void Server::readrequest(s_socket *s, HandlerFactory *factory){
    /*
        核心迴圈
    */
    while( 1 ){
        /*
            step 1: 讀取訊息
        */
        string res = "";
        res = s->readmessage();
        if(res == ""){
            break;  // 如果訊息為 null 即挑出迴圈
        }
        /*
            step 2:將訊息轉為 json 格式
        */
        json request = json::parse(res);
        /*
            step 3: 透過 HandlerFactory 取得應對的 Handler 處理要求
        */
        IHandler *handler =  factory->getHandler(request,s);
        if(handler == NULL){
            break;  // 如果 Handler 為 null 為錯誤要求 跳出迴圈
        }
        /*
            stpe 4: 轉交 Handler 處理要求
        */
        handler->handle();
    }
    s->closeConnection();
}
```

## <a name = "Monitor"></a> Monitor
1. `Monitor.hpp` 概觀
```cpp
class Monitor{
    /*
        略過以上說明過的設計樣式的函式
    */
    public:
        void addjob(json);              // request 要求新增工作時呼叫
        void notitfyschedualfinish();   // 通知 Server 排程器已結束排程
        json getjobstat();              // 取得 job queue 中的資訊
        void setnodelist();             // 設定 nodelist 資料 讀取計算節點 的 IP 和 PORT
        json getnodelist();             // 取得 計算節點的資料
        void setjobtoready(int,std::string);    // 將指定工作排入 ready queue
        void setjobtorunning(int,std::string);  // 將指定工作排入 running queue
        Node getnodeinfo(std::string);          // 取得指定的計算節點資料
        json getjobinfo(int);                   // 取得指定在 ready queue 中的 job 資訊
    private:
        std::map<std::string,Node> nodelist;    // 節點資料    
        std::map<int,json> joblist;             // job queue 未排入執行的工作
        std::map<int,json> readylist;           // ready queue 準備要執行的工作
        std::map<int,json> runninglist;         // running queue 正在執行的工作
        std::map<int,json> completelist;        // complete queue 已完成的工作
        /*
            用於防止多線程同時更改資料的保護鎖
        */
        std::mutex jobtex;
        std::mutex readytex;
        std::mutex runningtex;
        int jobcount = 0;       // 用於 job 的命名
        void notitfynewjob();   // 通知 Server 有新工作排入
};
```

## <a name = "Node"></a> Node
> 主要只用來存取計算節點的資訊，IP PORT NAME
> 未來可能還會有 CPU 核心數
```cpp
class Node{
    public:
    std::string getnodeip();
    std::string getnodeport();
    std::string getnodename();
    
    void setnodeip(std::string);
    void setnodeport(std::string);
    void setnodename(std::string);
    
    private:
    std::string nodeip,nodeport,nodename;
};

string Node::getnodeip(){
    return nodeip;
}

string Node::getnodeport(){
    return nodeport;
}

string Node::getnodename(){
    return nodename;
}

void Node::setnodeip(string ip){
    nodeip = ip;
}

void Node::setnodeport(string port){
    nodeport = port;
}

void Node::setnodename(string name){
    nodename = name;
}
```

## <a name = "Handler"></a> Handler
> 所有的 Handler 皆繼承 IHandler 介面 皆需要實作 handle() 函式 如以下所示
```cpp
class NewJobHandler : public IHandler{
    public:
    NewJobHandler(json,s_socket*);
    void handle();
    private:
    json new_job;
    s_socket *s;
};
```
> 以下列舉目前有的 Handler 
1. `NewJobHandler` : 用於處理新增工作
```cpp
NewJobHandler::NewJobHandler(json job, s_socket *socket){
    new_job = job;
    s = socket;
}

void NewJobHandler::handle(){
    // 將 JOB 透過 Monitor 新增入 joblist 中
    Monitor::GetInstance()->addjob(new_job);
}
```
2. `QueueStateHandler` :　用於處理回應 joblist 資訊 (排程器向Server要求Job queue中的資料)
```cpp
QueueStateHandler::QueueStateHandler(json request, s_socket *socket){
    s = socket;
    req_queue_state = request;
}

void QueueStateHandler::handle(){
    Message queuestate;
    /*
        step 1:透過 Monitor 取得 joblist 中的資訊
        step 2:封裝訊息標頭
        step 3:透過 socket 回應資料
    */
    queuestate.msg = Monitor::GetInstance()->getjobstat();
    queuestate.encode_Header("server","scheduler","queuestate");
    s->sendmessage(queuestate.encode_Message());
}
```
3. `NodeStateHandler` : 用於處理回應 nodelist 資訊 (排程器向Server要求計算節點的資料)
```cpp
NodeStateHandler::NodeStateHandler(json request, s_socket *socket){
    s = socket;
    req_node_state = request;
}

void NodeStateHandler::handle(){
    Message nodestate;
    /*
        step 1:透過 Monitor 取得 nodelist 中的資訊
        step 2:封裝訊息標頭
        step 3:透過 socket 回應資料
    */
    nodestate.msg = Monitor::GetInstance()->getnodelist();
    nodestate.encode_Header("server","scheduler","nodestate");
    s->sendmessage(nodestate.encode_Message());
}
```

4. `RunJobHandler` : 排程器排程結束後 要求Server 本次要執行的工作
```cpp

RunJobHandler::RunJobHandler(json req_run, s_socket *socket){
    req_runjob = req_run;
    s = socket;
}

void RunJobHandler::handle(){
    /*
        step 1:先將所有的指定的要執行的 job 放進 ready queue中
        要求的訊息如以下所示
        {
            "JOBID" : [0,0,1]
            "NODENAME" : [lab01,lab02,lab02]
        }
        此訊息代表 工作 0 需要 run 在 lab01,lab02 上
                  工作 1 需要 run 在 lab02 上
    */
    int task_count;
    if(req_runjob.count("JOBID") != 0){
        task_count = req_runjob["TASKCOUNT"].get<int>();
        for(int i = 0 ; i < task_count ; i++){
            Monitor::GetInstance()->setjobtoready(req_runjob["JOBID"][i].get<int>(),req_runjob["NODENAME"][i].get<std::string>());
        }
    }
    else
    {
        /*
            如果沒有工作要求 則通知排程器結束排程
            並結束處理
        */
        Monitor::GetInstance()->notitfyschedualfinish();
        return;
    }
    // 通知排程結束
    Monitor::GetInstance()->notitfyschedualfinish();
    
    // 迴圈 尋訪本次排程要求要執行的工作
    for(int i = 0 ; i < task_count ; i++){
        /*
            查看該工作是否有在 ready queue 中 如果沒有 代表已經在執行 尋訪下一個
        */
        json jobinfo = Monitor::GetInstance()->getjobinfo(req_runjob["JOBID"][i].get<int>());
        
        if(jobinfo == NULL){
            continue;
        }

        /*
            建立一個 client 端 socket
        */
        c_socket socket;
        Message message;
        message.msg = jobinfo;

        /*
            迴圈尋訪該工作要run 在那些計算節點上
        */
        for(int j = 0 ; j < (int)(jobinfo["RUNNODE"].size()) ; j++){
            string req_node = jobinfo["RUNNODE"][j];
            /*
                取得該節點資訊
            */
            Node node = Monitor::GetInstance()->getnodeinfo(req_node);
            
            /*
                和該節點進行連線
            */
            if(socket.setConnection(node.getnodeip(),node.getnodeport()) == 0){
                if(debug > 0){
                    if(debug == 1)
                        *debug_file << "RunJobHandler handle(): setConnection() ERROR! " << endl;
                    else if(debug == 2)
                        cout << "RunJobHandler handle(): setConnection() ERROR! " << endl;
                }
                continue;
            }
            if(socket.connect2server() == 0){
                if(debug > 0){
                    if(debug == 1)
                        *debug_file << "RunJobHandler handle(): connect2server() ERROR! " << endl;
                    else if(debug == 2)
                        cout << "RunJobHandler handle(): connect2server() ERROR! " << endl;
                }
                continue;
            }
            
            /*
                如果連線成功 則設置該節點為 MOTHERNODE 跳出迴圈
            */
            message.msg["MOTHERNODE"] = node.getnodename();
            break;
        }

        /*
            如果連線不到任何節點 繼續下一個工作的尋訪
            未來要設置能讓該工作 塞回 job queue 等待
        */
        if(message.msg.count("MOTHERNODE") == 0){
            // push_font to the joblist ??
            continue;
        }
            
        /*
            將其他的所有要執行的節點資料封裝入訊息中
            讓 MOTHERNODE 可再告知其他節點要執行工作
        */
        for(int j = 0 ; j < (int)(jobinfo["RUNNODE"].size()) ; j++){
            string req_node = jobinfo["RUNNODE"][j];
            Node node = Monitor::GetInstance()->getnodeinfo(req_node);
            message.msg["RUNNODEIP"].push_back(node.getnodeip());
            message.msg["RUNNODEPORT"].push_back(node.getnodeport());
        }
        
        /*
            封裝訊息標頭
        */
        message.encode_Header("server","mom","runjob");
        /*
            訊息結果範例:
            {
                "SENDER":"server",
                "RECEIVER":"mom",
                "REQUEST":"runjob",
                "SCRIPT":["echo do","echo done"],
                "ENV":
                    {
                        "HOME":"/acs103",
                        "HOSTNAME":"lab02",
                        "USER":"acs103",
                        .....
                    }
                "MOTHERNODE" : "lab01",
                "RUNNODE" : ["lab01","lab02"],
                "RUNNODEIP" : ["127.0.0.1","127.0.0.2"],
                "RUNNODEPORT" : ["5003","5004d"]
            }
        */
        socket.send(message.encode_Message());
        socket.closeConnection();
        /*
            將 ready queue 中 該工作排入 running queue中，並告知 mothor node
        */
        Monitor::GetInstance()->setjobtorunning(req_runjob["JOBID"][i].get<int>(),message.msg["MOTHERNODE"].get<string>());
    }
}
```

## <a name = "HandlerFactory"></a> HandlerFactory : 根據不同的 request 給予對應的 Handler

```cpp
IHandler *HandlerFactory::getHandler(json request,s_socket *s)
{
    /*
        根據不一樣的 request 訊息 return 對應的 Handler
    */
    //NewJobHandler
    if(request["SENDER"].get<std::string>()   == "subjob"       &&
       request["RECEIVER"].get<std::string>() == "server"       && 
       request["REQUEST"].get<std::string>()  == "newjob"){
        return new NewJobHandler(request,s);
    }
    //NewJobHandler

    //RunJobHandler
    if(request["SENDER"].get<std::string>()   == "scheduler"    &&
       request["RECEIVER"].get<std::string>() == "server"       && 
       request["REQUEST"].get<std::string>()  == "runjob"){
        return new RunJobHandler(request,s);
    }
    //RunJobHandler

    //ERROR
    
    return NULL;
}
```