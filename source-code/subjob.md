# Subjob
### 負責將script檔案解析並發送至server要求工作執行

- [main](#main)
- [subjobservice](#subjob_service)
- Dependency
    - [Message](./library.md#Message)
    - [cc_socket](./library.md#ccsocket)

## <a name = "main"></a> main
```cpp
int main(int argc, char **argv){

    string script_name;
    if(argc > 0 && argv[1] != NULL){
        script_name = std::string(argv[1]);
    }
    else{
        cout << "Subjob ---> Error :No input script file." << endl;
        exit(1);
    }
    
    /*
        建立一個要封裝給Server訊息的物件
    */
    Message newjob;
    newjob.initMessage();

    /*
        subjob_service 提供封裝訊息的服務 
        只需要提供 script 的檔名以及要封裝的訊息位址
    */
    subjob_service srv;
    srv.creatjob(&newjob, script_name);

    /*
        讀取subjob.con檔案中server的IP和PORT
    */
    ifstream f("subjob.con");
    string ip,port;
    f >> ip;
    f >> port;

    /*
        CC_socket : client socket
        只需提供要連到的server IP 以及 PORT 就可設定連線 -->setConnection(ip,port)
    */
    cc_socket csock;
    if(csock.setConnection(ip,port) != 1 ){
        exit(1);
    }

    /*
        設定連線成功後 即可向Server要求連線 connect2server()
    */
    if(csock.connect2server() != 1 ){
        exit(1);
    }

    /*
        連線成功後 相Message 封裝 透過socket 傳送給Server 並結束連線
    */
    csock.send(newjob.encode_Message());
    csock.closeConnection();

}
```

## <a name = "subjobservice"></a> subjob_service

1. `subjob_service.hpp` 概觀
```cpp
class subjob_service{
    private:
    //setenv_attrubute 封裝環境變數資料
    void setenv_attrubute(Message*);
    //parse_script 將script檔案逐行parse到訊息中
    void parse_script(Message*,std::string);
    public:
    //提供main函式要求封裝newjob訊息
    void creatjob(Message*,std::string);
};
```
2. `setenv_attrubute()`:封裝環境變數資料
```cpp
void subjob_service::setenv_attrubute(Message *j){
    /*
        陣列放置需要的環境變數
    */
    string env_need[]={"HOME","HOSTNAME","PATH","TZ","USER","MAIL","SHELL","LANG"};
    /*
        for迴圈逐項讀取環境變數 並封裝到訊息中
    */
    char *envdata;
	for(int i=0 ; i < (int)(sizeof(env_need)/sizeof(env_need[0])) ; i++)
	{
		if( (envdata = getenv(env_need[i].c_str())) ){
			j->msg["ENV"][env_need[i]] = envdata;
		}
	}
    /*
        封裝後結果範例: 
        {
            "ENV":
                {
                    "HOME":"/acs103",
                    "HOSTNAME":"lab02",
                    "USER":"acs103",
                    .....
                }
        }
    */
}
``` 

3. `parse_script()`:將script檔案逐行parse到訊息中
```cpp
void subjob_service::parse_script(Message *j,string script_name){
    /*
        透過stat() 可檢視是否能讀取該檔案
        stat()為linux函式可查詢其功用
    */
    struct stat statbuf;
    if (stat(script_name.c_str(), &statbuf) < 0){
        std::cout << "err: script file cannot be loaded\n";
        exit(1);
    }
    /*
        透過觀看stat後的回傳結果使用S_ISREG(state.st_mode) 可察看檔案是否為文件
    */
    if (!S_ISREG(statbuf.st_mode)){
        cout <<"err: script not a file\n";
        exit(1);
    }

    /*
        檢驗完畢後將script導入 ifstream 作為輸入
    */
    ifstream f(script_name);
    string line;
    int count = 0;
    /*
        透過迴圈逐行導入訊息
    */
    while(getline(f,line)){
        j->msg["SCRIPT"][count] = line;
        count++;
    }
    /*
        封裝後結果範例:
        {
            "SCRIPT":["echo do","echo done"]
        }
    */
}
```

4. `creatjob()` :提供main函式要求封裝newjob訊息
```cpp
void subjob_service::creatjob(Message *j,string script_name){
    /*
        step1: setenv_attrubute() -->封裝環境變數
        step2: parse_script()     -->封裝script檔
    */
    setenv_attrubute(j);
    parse_script(j,script_name);

    /*
        step3: Message.encode_Header() -->封裝訊息標頭
        傳送者 : subjob
        接收者 : server
        要求式 : newjob
    */
    string sender("subjob");
    string receiver("server");
    string request("newjob");
    j->encode_Header(sender,receiver,request);
    /*
        最終結果訊息範例:
        {
            "SENDER":"subjob",
            "RECEIVER":"server",
            "REQUEST":"newjob",
            "SCRIPT":["echo do","echo done"],
            "ENV":
                {
                    "HOME":"/acs103",
                    "HOSTNAME":"lab02",
                    "USER":"acs103",
                    .....
                }
        }
    */
}
```