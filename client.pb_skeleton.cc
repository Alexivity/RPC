#include "user.srpc.h"
#include <iostream>
#include <string>
#include "srpc/server.h"
class UserServiceImpl : public UserService::Service
{
public:
    void Signup(ReqSignup *request, RespSignup *response, srpc::RPCContext *ctx) override
    {
        // 获取请求中的用户名和密码
        std::string username = request->username();
        std::string password = request->password();

        std::cout << "Received Signup Request:" << std::endl;
        std::cout << "Username: " << username << std::endl;
        std::cout << "Password: " << password << std::endl;

        // 模拟处理逻辑
        if (!username.empty() && !password.empty())
        {
            std::cout << "Signup successful for user: " << username << std::endl;
        }
        else
        {
            std::cout << "Signup failed: Invalid username or password" << std::endl;
        }

    }
};

int main()
{
    const char *ip = "127.0.0.1";
    unsigned short port = 1412;

    // 创建服务实例
    UserServiceImpl service;

    // 启动 RPC 服务
    srpc::RPCServer server;
    if (server.add_service(&service) == 0)
    {
        std::cout << "RPC Server started at " << ip << ":" << port << std::endl;
        server.start(ip, port);
        server.wait(); // 阻塞等待
    }
    else
    {
        std::cerr << "Failed to start RPC Server" << std::endl;
        return -1;
    }

    return 0;
}
