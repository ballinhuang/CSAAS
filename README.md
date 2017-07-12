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
<node1's ip> <node1's port> <node1's name> <node1's core>
<node2's ip> <node2's port> <node2's name> <node2's core>
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

# Example
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

