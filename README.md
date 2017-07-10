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
### server
```
$ vim server.con
<server's ip> <server's port>

$ vim scheduler.con
<scheduler's ip> <scheduler's port>

$ vim node.con
<node's ip> <node's port> <node's name>
<node's ip> <node's port> <node's name>
...

```

### scheduler
```
$ vim scheduler.con
<scheduler's ip> <scheduler's port>
```

### mom
```
$ vim server.con
<server's ip> <server's port>
$ vim mom.con
<mom's ip> <mom's port>
```

### subjob
```
$ vim subjob.con
<server's ip> <server's port>
```

# Exampel
(-command) --> optional

```
$ cd server
$ ./server (-odebug)
$ cd ..
$ cd scheduler
$ ./scheduler -mode FIFO (-odebug)
$ cd ..
$ cd mom
$ ./mom (-odebug)
$ cd ..

$ cd subjob
$ ./subjob <your script filename>
```

