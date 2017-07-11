# Library
### 為各部件間抽離出來的共用模組

## List

- [Socket](#Socket)
    - [cc_socket](#ccsocket) : Client socket with send and receive.
    - [s_socket](#ssocket) : Server socket with send and receive.
    - [c_socket](#csocket) : Client socket with send but without receive.
- [Message](#Message) : Use to encode message send between socket.
- [ThreadPool](https://github.com/progschj/ThreadPool) : Link to progschj/ThreadPool
    - [ThreadPool example](#ThreadPool)
- [json](https://github.com/nlohmann/json) : Link to nlohmann/json
    - [json example](#json)


## <a name="Socket"></a> Socket
### <a name="ccsocket"></a> cc_socket
1. `cc_socket.hpp` 概觀
```c++
class cc_socket{
    public:
    int setConnection(std::string,std::string); // 設定client socket 的連線
    int connect2server();                       // 連線至server端
    void send(std::string);                     // 發送訊息給server
    void closeConnection();                     // 關閉連線
    std::string receive();                      // 接收訊息
    
    private:
    /*
        hendshack 是用來雙方收送端的socket協議本次傳送的訊息大小
    */
    int receivehendshack();
    void sendhendshack(int);

    struct sockaddr_in server_addr; // sockaddr_in 結構用來設置要連線的server port,ip
    int sock_fd;
};
```
2. `setConnection(std::string,std::string)`:設定client socket 的連線，傳入欲連線的server ip和port進行設定.
```c++
int cc_socket::setConnection(string ip, string port){
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    server_addr.sin_port = htons(stoi(port));
    server_addr.sin_family = AF_INET;
    /*
        建立 socket file descriptor
        成功回傳:1
        失敗回傳:0
    */
    if( (sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        cout << "subjob client socket creat error !" << endl;
        return 0;
    }
    return 1;
}
```
3. `connect2server()`:連線至Server端
```c++
int cc_socket::connect2server(){
    /*
        透過connect()連線至Server
        成功回傳:1
        失敗回傳:0
    */
    if(connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        cout << "subjob client socket connect error !" << endl;
        return 0;
    }
    return 1;
}
```
4. `hendshack`:收送端的socket協議本次傳送的訊息大小
> sendhendshack(int size):透過 write() 發送訊息大小
```c++
void cc_socket::sendhendshack(int size){
    char num[10];
    sprintf(num,"%d",size);
    write(sock_fd,num,sizeof(num));
}
```
> receivehendshack():透過 read() 接收訊息大小 並轉為INT回傳
```c++
int cc_socket::receivehendshack(){
    char num[10];
    read(sock_fd,num,sizeof(num));
    int size = atoi(num);
    return size;
}
```
5. `send and receive`:接收與發送訊息至Server
> send(string msg)
```c++
void cc_socket::send(string msg){
    /*
        step 1:透過 sendhendshack() 告知Server訊息大小
        step 2:宣告 char *buf 使用 malloc 配置其大小
        step 3:清空 buf 中的所有資料
        step 4:透過 strcpy() 將訊息放入
        step 5:透過 write() 將訊息送給Server
        step 6:清空 buf 並透過 free() 釋放空間
    */
    sendhendshack(msg.size());
    char *buf = (char*)malloc(sizeof(char) * (msg.size()+1));
    memset(buf,0,msg.size()+1);
    strcpy(buf,msg.c_str());
    write(sock_fd,buf,msg.size());
    memset(buf,0,msg.size()+1);
    free(buf);
}
```
> receive()
```c++
string cc_socket::receive(){
    /*
        step 1:透過 receivehendshack() 接收訊息大小
        step 2:宣告 char *buf 使用 malloc 配置其大小
        step 3:清空 buf 中的所有資料
        step 4:透過 read() 接收訊息至buf
        step 5:將 buf 轉為 string      
        step 6:清空 buf 並透過 free() 釋放空間
        step 7:回傳 string
    */
    int size = receivehendshack();
    char *buf = (char*)malloc(sizeof(char) * (size+1));
    memset(buf,0,size+1);
    read(sock_fd,buf,(size_t)size);
    string result = buf;
    memset(buf,0,size+1);
    free(buf);
    return result;
}
```
### <a name="ssocket"></a> s_socket
1. `s_socket.hpp` 概觀
```c++
class s_socket{
    public:
    int setConnection(std::string,std::string); // 設定Server socket
    int acceptClinet();                         // 接受Client 連線
    std::string readmessage();                  // 接收訊息
    void closeConnection();                     // 結束連線
    void closebind();                           // 關閉socketfd的綁定
    void setacceptreuse();                      // 設定 accept socket_fd 可重用
    void sendmessage(std::string);              // 傳送訊息
    private:
    /*
        hendshack 是用來雙方收送端的socket協議本次傳送的訊息大小
    */
    int receivehendshack();
    void sendhendshack(int size);

    struct sockaddr_in server_addr;             // server的 sockaddr_in結構 用來存取 server的 ip,port                           
    struct sockaddr_in client_addr; 	        // accept的 client sockaddr_in結構 用來存取 client的 ip,port                           
    int sock_fd,conn_port;                      // socket    --> socket()得到的檔案描述符
                                                // conn_port --> accept()得到的檔案描述符
};
```
2. `setConnection(std::string,std::string)`: 設定Server socket 
```c++
int s_socket::setConnection(string ip, string port){

    /*
        step 1: 設置 Server socket的 IP 和 Port
        step 2: 透過 socket() 建立 socket file descriptor
        step 3: 設置 sock_fd 為可重用
        step 4: 將 sock_fd 透過 bind() 綁定 server_addr
        step 5: 透過 listent() 將 sock_fd 設置接聽

        成功回傳:1
        失敗回傳:0
    */
    memset(&server_addr, '\0', sizeof(struct sockaddr_in));
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(stoi(port));
    server_addr.sin_family = AF_INET;

    if( (sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        cout << "Server socket creat error !" << endl;
        return 0;
    }

    int on = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
    {
        close(sock_fd);
        sock_fd = -3;
        return 0;
    }

    if(bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        cout << "Server socket bind error !" << endl;
        return 0;
    }

    if(listen(sock_fd, 256) == -1) {
        cout << "Server socket listen error !" << endl;
        return 0;
    }
    return 1;
}
```
3. `int acceptClinet()` : 接受Client 連線
```c++
int s_socket::acceptClinet(){
    /*
        透過 accept() 接受 Client的連線 並將 accept 後所得的 file descriptor 存於 conn_port

        成功回傳:1
        失敗回傳:0
    */
    memset(&client_addr, '\0', sizeof(struct sockaddr_in));
    int sin_size = sizeof(struct sockaddr_in);
    if((conn_port = accept(sock_fd, (struct sockaddr *)&client_addr, (socklen_t*)&sin_size)) == -1){
        return 0;
    }
    return 1;
}
```
4. `hendshack` : 協議本次傳送的訊息大小 *原理同cc_socket*
> receivehendshack()
```c++
int s_socket::receivehendshack(){
    char num[10];
    read(conn_port,num,sizeof(num));
    int size = atoi(num);
    return size;
}
```
> sendhendshack()
```c++
void s_socket::sendhendshack(int size){
    char num[10];
    sprintf(num,"%d",size);
    write(conn_port,num,sizeof(num));
}
```
5. `send and receive` : 傳送與接收訊息 *原理同cc_socket*
> readmessage()
```c++
string s_socket::readmessage(){
    int size = receivehendshack();
    string result = "";
    char *buf = (char*)malloc(sizeof(char) * (size+1));
    memset(buf,0,size+1);
    read(conn_port,buf,(size_t)size);
    result = buf;
    memset(buf,0,size+1);
    free(buf);
    return result;
}
```
> sendmessage()
```c++
void s_socket::sendmessage(string msg){
    sendhendshack(msg.size());
    char *buf = (char*)malloc(sizeof(char) * (msg.size()+1));
    memset(buf,0,msg.size()+1);
    strcpy(buf,msg.c_str());
    write(conn_port,buf,msg.size());
    memset(buf,0,msg.size()+1);
    free(buf);
}
```
6. `closebind()` : 關閉 socket 的綁定 使之能創立下一個socket接收其他連線
```c++
void s_socket::closebind(){
    /*
        透過 close() 將 sock_fd 關閉
    */
    close(sock_fd);
}
```
7. `closeConnection()` : 關閉 socket 連線
```c++
void s_socket::closeConnection(){
    /*
        透過 close() 將 conn_port 關閉
    */
    close(conn_port);
}
```
8. `setacceptreuse()` : 使 accept() 的 socket file descriptor 可重用
```c++
void s_socket::setacceptreuse(){
    int sockoptval = 1;
    setsockopt(conn_port, SOL_SOCKET, SO_REUSEADDR, (void *)&sockoptval, sizeof(sockoptval));
}
```
### <a name="csocket"></a> c_socket
*原理同 cc_socket 只差在沒有接收功能*
*可能會與 cc_socket直接整併*

## <a name="Message"></a> Message
1. `Message.hpp` 概觀
```c++
#include<json.hpp>
// for convenience
using json = nlohmann::json;

class Message{
    public:
    void initMessage();                                     // 初始化 成員變數 msg
    std::string encode_Message();                           // 將 成員變數 msg 轉為 string 回傳
    void encode_Header(std::string,std::string,std::string);// 封裝訊息標頭
    json msg;   // 成員變數 msg 以 json格式 存取訊息
};
```
2. `initMessage()`: 初始化成員變數 msg
```c++
void Message::initMessage(){
    msg.clear();
}
```
3. `encode_Message()`: 將成員變數 msg 轉為 string 回傳
```c++
string Message::encode_Message(){
    string result = msg.dump();
    return result;
}
```
4. `encode_Header(std::string,std::string,std::string)` 封裝訊息標頭
```c++
void Message::encode_Header(string sender, string receiver, string request){
    msg["SENDER"] = sender;
    msg["RECEIVER"] = receiver;
    msg["REQUEST"] = request;
    /*
        標頭範例:
        {
            "SENDER":"server",
            "RECEIVER":"scheduler",
            "REQUEST":"do_schedule"
        }
    */
}
```
## <a name="ThreadPool"></a> ThreadPool example
![ThreadPool](Thread_pool.png)

> Example code
```c++
#include <iostream>
#include <vector>
#include <chrono>
#include <cstdio>
#include "ThreadPool.h"

void f(int s){
    printf("job %d creat!\n",s);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	printf("job %d down!\n",s);
}

int main()
{
    /*
        宣告三條線程
    */
	ThreadPool pool(3);
    /*
        將 function f 封裝入 pool 一封裝就會開始執行
    */
	for(int i = 0; i < 15; ++i) {
		int arg = i;
		pool.enqueue(&f,arg);
	}
	return 0;
}
```

## <a name="json"></a> json example

> Example

```c++
// create an empty structure (null)
json j;

// add a number that is stored as double (note the implicit conversion of j to an object)
j["pi"] = 3.141;

// add a Boolean that is stored as bool
j["happy"] = true;

// add a string that is stored as std::string
j["name"] = "Niels";

// add another null object by passing nullptr
j["nothing"] = nullptr;

// add an object inside the object
j["answer"]["everything"] = 42;

// add an array that is stored as std::vector (using an initializer list)
j["list"] = { 1, 0, 2 };

// add another object (using an initializer list of pairs)
j["object"] = { {"currency", "USD"}, {"value", 42.99} };
```

> result

```javascript
j = 
{
  "pi" : 3.141,
  "happy" : true,
  "name" : "Niels",
  "nothing" : nullptr,
  "answer":
    {"everything" : 42},
  "list" :　[1, 0, 2],
  "object" : 
    {
        "currency" : "USD",
        "value" : 42.99
    }
}
```