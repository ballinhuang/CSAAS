# Library
### 為各部件間抽離出來的共用模組

## List

- [Socket](#Socket)
    - [cc_socket](#cc_socket) : Client socket with send and receive.
    - [s_socket](#s_socket) : Server socket with send and receive.
    - [c_socket](#c_socket) : Client socket with send but without receive.
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

    struct sockaddr_in server_addr; // 
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
### <a name="csocket"></a> c_socket

## <a name="Message"></a> Message
## <a name="ThreadPool"></a> ThreadPool example
## <a name="json"></a> json example

