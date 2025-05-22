

## 环境
-lwfrest -lworkflow -lssl -lcrypto -lcrypt  -lpthread -lSimpleAmqpClient -lsrpc -lprotobuf -lalibabacloud-oss-cpp-sdk -lcurl -llz4 -lsnappy -lppconsul
以上所有库自行搜索安装
## 编译
Makefile 直接make 

## 测试
配置并开启rabbitmq oss nginx consul
运行consumer wfserver
* 请使用浏览器访问 http://ip:8888/user/signup
* 温馨提醒: 记得更换成自己的 ip !!!

## Nginx
nginx配置自行搜索
代理转发给wfserver


protoc --cpp_out=./ user.proto
srpc_generator protobuf user.proto ./
```

## 注册中心consul

consul的部署

1. 启动consul服务器

// HTTP:8500 LAN:8301 WAN:8302 DNS:8600
sudo docker run --hostname consulsvr1 --name consul_node_1 -d -p 8500:8500 -p 8301:8301 -p  8302:8302 -p 8600:8600 consul agent -server -bootstrap-expect 2 -ui -bind=0.0.0.0 -client=0.0.0.0
```

2. 查看分配的IP地址  
```
sudo docker inspect --format '{{.NetworkSettings.IPAddress}}' consul_node_1
```

3. 启动另外两个节点, 加入集群(注意与2的IP地址保持一致)
```
sudo docker run --hostname consulsvr2 --name consul_node_2 -d -p 8501:8500 consul agent -server -ui -bind=0.0.0.0 -client=0.0.0.0 -join 172.17.0.3

sudo docker run --hostname consulsvr3 --name consul_node_3 -d -p 8502:8500 consul agent -server -ui -bind=0.0.0.0 -client=0.0.0.0 -join 172.17.0.3
```

4. 查看集群状态
```
sudo docker exec -t consul_node_1 consul members
```


