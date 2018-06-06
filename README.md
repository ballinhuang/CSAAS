# Configurable Scheduling as a Service(CSaaS)

### Models

model name | utilization
------------ | -------------
server | receive job, notice scheduler, resource management
mom | execute the job
scheduler | determine which job to be run
subjob | when client send the script, parse it to server

# Compile
```
$ make all
```

# Configure file need
### server daemon 需要讀取的節點檔案
```
$ vim node.con
<node1's ip> <node1's port> <node1's name> <node1's core>
<node2's ip> <node2's port> <node2's name> <node2's core>
...

```

### subjob
```
$ vim subjob.con
<server's ip> <server's port>
```
or
```
$./subjob -i <server's ip> -p <server's port> <filename>
```

# 開始使用

### server daemon

```
$ cd server
$ ./server -i [serverip] -p [serverport] -si [schesulerip] -sp [schedulerport] (-odebug)
```

### scheduler daemon
```
$ cd scheduler
$ ./scheduler -mode FIFO -i [schesulerip] -p [schedulerport] (-odebug)
```

### mom daemon (計算節點都要啟動這個)
```
$ cd mom
$ ./mom -i [momip] -p [momport] (-odebug)
```
### subjob (提交工作)
```
$ cd subjob
$ ./subjob -i [serverip] -p [serverport] <script filename>
```

