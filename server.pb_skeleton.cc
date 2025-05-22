#include "user.srpc.h"
#include "workflow/WFFacilities.h"
#include <ppconsul/ppconsul.h>
using namespace srpc;
using namespace ppconsul;
static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int signo)
{
	wait_group.done();
}

static void signup_done(RespSignup *response, srpc::RPCContext *context)
{
    int code = response->code();
    std::string msg = response->msg();
    std::cout << "code: " << code << std::endl;
    std::cout << "msg: " << msg << std::endl;
}

class UserServiceServiceImpl : public UserService::Service
{
public:

	void Signup(ReqSignup *request, RespSignup *response, srpc::RPCContext *ctx) override
	{
		GOOGLE_PROTOBUF_VERIFY_VERSION;

    // 使用 Consul 查询服务地址和端口
		Consul consul("127.0.0.1:8500", ppconsul::kw::dc = "dc1"); // 连接到 Consul
		agent::Agent agent(consul);
		// 查询服务信息
		auto services = agent.services();
		auto it = services.find("SignupService1");
		if (it == services.end())
		{
			std::cerr << "Service not found in Consul!" << std::endl;
			return ;
		}

		// 获取服务地址和端口
		const std::string &ip = it->second.address;
		unsigned short port = it->second.port;

		std::cout << "Connecting to service at " << ip << ":" << port << std::endl;

		// 创建客户端
		UserService::SRPCClient client(ip.c_str(), port);

		// 构造请求
		ReqSignup signup_req;
		signup_req.set_username("admin");
		signup_req.set_password("123");

		// 调用服务
		client.Signup(&signup_req, signup_done);

		wait_group.wait();
		google::protobuf::ShutdownProtobufLibrary();
	}
};

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	unsigned short port = 1412;
	SRPCServer server;

	UserServiceServiceImpl userservice_impl;
	server.add_service(&userservice_impl);

	server.start(port);
	wait_group.wait();
	server.stop();
	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
