# IoModel


# 同步阻塞 IOModelType_spst

# 多进程并发 IOModelType_mpst todo

# 多线程并发 IOModelType_spmt todo

# io 复用

# select 

# poll

# kqueue(epoll)


# 执行
1. 在IoModel下执行  `cmake .`

2. `cd bin`

3. 启动服务器,端口7097 类型5代表io模型为epoll(kqueue)

    `./io_model_server 7097 5 ` 

   ```bash
   (base) ➜ bin (main) ✗ ./io_model_server 7097 5 
   port:7097
   htons(port):47387
   recvived from 127.0.0.1 at port 51207, sockfd:3, clientfd:5
   recv: helllo, 6 Bytes
   recvived from 127.0.0.1 at port 51228, sockfd:3, clientfd:6
   recv: c2 报道, 9 Bytes
   
   ```

   

4. 启动客户端 

   `./io_model_client 7097 `

```bash
(base) ➜ bin (main) ✗ ./io_model_client 7097
port:7097
htons(port):47387
请输入要发送的数据 (输入'quit'退出)：
helllo

(base) ➜ bin (main) ✗ ./io_model_client 7097
port:7097
htons(port):47387
请输入要发送的数据 (输入'quit'退出)：
c2 报道 

```

