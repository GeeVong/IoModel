#!/bin/bash

# 可执行文件路径
client_executable="./client"

# 服务器主机和端口
server_host="127.0.0.1"
server_port="8080"

# 主循环
while true; do
  # 读取用户输入
  read -p "请输入消息: " message

  # 调用可执行文件并传递消息作为参数
  $client_executable $server_port "$message"
done