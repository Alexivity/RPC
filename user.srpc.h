#pragma once
#include <stdio.h>
#include <string>
#include "srpc/rpc_define.h"
#include "user.pb.h"

namespace UserService
{

/*
 * Server codes
 * Generated by SRPC
 */

class Service : public srpc::RPCService
{
public:
	// please implement these methods in server.cc

	virtual void Signup(ReqSignup *request, RespSignup *response,
					srpc::RPCContext *ctx) = 0;

public:
	Service();
};

/*
 * Client codes
 * Generated by SRPC
 */

using SignupDone = std::function<void (RespSignup *, srpc::RPCContext *)>;

class SRPCClient : public srpc::SRPCClient
{
public:
	void Signup(const ReqSignup *req, SignupDone done);
	void Signup(const ReqSignup *req, RespSignup *resp, srpc::RPCSyncContext *sync_ctx);
	WFFuture<std::pair<RespSignup, srpc::RPCSyncContext>> async_Signup(const ReqSignup *req);

public:
	SRPCClient(const char *host, unsigned short port);
	SRPCClient(const struct srpc::RPCClientParams *params);

public:
	srpc::SRPCClientTask *create_Signup_task(SignupDone done);
};

class SRPCHttpClient : public srpc::SRPCHttpClient
{
public:
	void Signup(const ReqSignup *req, SignupDone done);
	void Signup(const ReqSignup *req, RespSignup *resp, srpc::RPCSyncContext *sync_ctx);
	WFFuture<std::pair<RespSignup, srpc::RPCSyncContext>> async_Signup(const ReqSignup *req);

public:
	SRPCHttpClient(const char *host, unsigned short port);
	SRPCHttpClient(const struct srpc::RPCClientParams *params);

public:
	srpc::SRPCHttpClientTask *create_Signup_task(SignupDone done);
};

class BRPCClient : public srpc::BRPCClient
{
public:
	void Signup(const ReqSignup *req, SignupDone done);
	void Signup(const ReqSignup *req, RespSignup *resp, srpc::RPCSyncContext *sync_ctx);
	WFFuture<std::pair<RespSignup, srpc::RPCSyncContext>> async_Signup(const ReqSignup *req);

public:
	BRPCClient(const char *host, unsigned short port);
	BRPCClient(const struct srpc::RPCClientParams *params);

public:
	srpc::BRPCClientTask *create_Signup_task(SignupDone done);
};

class TRPCClient : public srpc::TRPCClient
{
public:
	void Signup(const ReqSignup *req, SignupDone done);
	void Signup(const ReqSignup *req, RespSignup *resp, srpc::RPCSyncContext *sync_ctx);
	WFFuture<std::pair<RespSignup, srpc::RPCSyncContext>> async_Signup(const ReqSignup *req);

public:
	TRPCClient(const char *host, unsigned short port);
	TRPCClient(const struct srpc::RPCClientParams *params);

public:
	srpc::TRPCClientTask *create_Signup_task(SignupDone done);
};

class TRPCHttpClient : public srpc::TRPCHttpClient
{
public:
	void Signup(const ReqSignup *req, SignupDone done);
	void Signup(const ReqSignup *req, RespSignup *resp, srpc::RPCSyncContext *sync_ctx);
	WFFuture<std::pair<RespSignup, srpc::RPCSyncContext>> async_Signup(const ReqSignup *req);

public:
	TRPCHttpClient(const char *host, unsigned short port);
	TRPCHttpClient(const struct srpc::RPCClientParams *params);

public:
	srpc::TRPCHttpClientTask *create_Signup_task(SignupDone done);
};

///// implements detials /////

inline Service::Service(): srpc::RPCService("UserService")
{
	this->srpc::RPCService::add_method("Signup",
		[this](srpc::RPCWorker& worker) ->int {
			return ServiceRPCCallImpl(this, worker, &Service::Signup);
		});
}

inline SRPCClient::SRPCClient(const char *host, unsigned short port):
	srpc::SRPCClient("UserService")
{
	struct srpc::RPCClientParams params = srpc::RPC_CLIENT_PARAMS_DEFAULT;
	
	params.host = host;
	params.port = port;
	this->srpc::SRPCClient::init(&params);
}

inline SRPCClient::SRPCClient(const struct srpc::RPCClientParams *params):
	srpc::SRPCClient("UserService")
{
	const struct srpc::RPCClientParams *temp = params;
	struct srpc::RPCClientParams temp_params;
	
	this->srpc::SRPCClient::init(temp);
}

inline void SRPCClient::Signup(const ReqSignup *req, SignupDone done)
{
	auto *task = this->create_rpc_client_task("Signup", std::move(done));

	task->serialize_input(req);
	task->start();
}

inline void SRPCClient::Signup(const ReqSignup *req, RespSignup *resp, srpc::RPCSyncContext *sync_ctx)
{
	auto res = this->async_Signup(req).get();

	if (resp && res.second.success)
		*resp = std::move(res.first);

	if (sync_ctx)
		*sync_ctx = std::move(res.second);
}

inline WFFuture<std::pair<RespSignup, srpc::RPCSyncContext>> SRPCClient::async_Signup(const ReqSignup *req)
{
	using RESULT = std::pair<RespSignup, srpc::RPCSyncContext>;
	auto *pr = new WFPromise<RESULT>();
	auto fr = pr->get_future();
	auto *task = this->create_rpc_client_task<RespSignup>("Signup", srpc::RPCAsyncFutureCallback<RespSignup>);

	task->serialize_input(req);
	task->user_data = pr;
	task->start();
	return fr;
}

inline srpc::SRPCClientTask *SRPCClient::create_Signup_task(SignupDone done)
{
	return this->create_rpc_client_task("Signup", std::move(done));
}

inline SRPCHttpClient::SRPCHttpClient(const char *host, unsigned short port):
	srpc::SRPCHttpClient("UserService")
{
	struct srpc::RPCClientParams params = srpc::RPC_CLIENT_PARAMS_DEFAULT;
	
	params.host = host;
	params.port = port;
	this->srpc::SRPCHttpClient::init(&params);
}

inline SRPCHttpClient::SRPCHttpClient(const struct srpc::RPCClientParams *params):
	srpc::SRPCHttpClient("UserService")
{
	const struct srpc::RPCClientParams *temp = params;
	struct srpc::RPCClientParams temp_params;
	
	this->srpc::SRPCHttpClient::init(temp);
}

inline void SRPCHttpClient::Signup(const ReqSignup *req, SignupDone done)
{
	auto *task = this->create_rpc_client_task("Signup", std::move(done));

	task->serialize_input(req);
	task->start();
}

inline void SRPCHttpClient::Signup(const ReqSignup *req, RespSignup *resp, srpc::RPCSyncContext *sync_ctx)
{
	auto res = this->async_Signup(req).get();

	if (resp && res.second.success)
		*resp = std::move(res.first);

	if (sync_ctx)
		*sync_ctx = std::move(res.second);
}

inline WFFuture<std::pair<RespSignup, srpc::RPCSyncContext>> SRPCHttpClient::async_Signup(const ReqSignup *req)
{
	using RESULT = std::pair<RespSignup, srpc::RPCSyncContext>;
	auto *pr = new WFPromise<RESULT>();
	auto fr = pr->get_future();
	auto *task = this->create_rpc_client_task<RespSignup>("Signup", srpc::RPCAsyncFutureCallback<RespSignup>);

	task->serialize_input(req);
	task->user_data = pr;
	task->start();
	return fr;
}

inline srpc::SRPCHttpClientTask *SRPCHttpClient::create_Signup_task(SignupDone done)
{
	return this->create_rpc_client_task("Signup", std::move(done));
}

inline BRPCClient::BRPCClient(const char *host, unsigned short port):
	srpc::BRPCClient("UserService")
{
	struct srpc::RPCClientParams params = srpc::RPC_CLIENT_PARAMS_DEFAULT;
	
	params.host = host;
	params.port = port;
	this->srpc::BRPCClient::init(&params);
}

inline BRPCClient::BRPCClient(const struct srpc::RPCClientParams *params):
	srpc::BRPCClient("UserService")
{
	const struct srpc::RPCClientParams *temp = params;
	struct srpc::RPCClientParams temp_params;
	
	this->srpc::BRPCClient::init(temp);
}

inline void BRPCClient::Signup(const ReqSignup *req, SignupDone done)
{
	auto *task = this->create_rpc_client_task("Signup", std::move(done));

	task->serialize_input(req);
	task->start();
}

inline void BRPCClient::Signup(const ReqSignup *req, RespSignup *resp, srpc::RPCSyncContext *sync_ctx)
{
	auto res = this->async_Signup(req).get();

	if (resp && res.second.success)
		*resp = std::move(res.first);

	if (sync_ctx)
		*sync_ctx = std::move(res.second);
}

inline WFFuture<std::pair<RespSignup, srpc::RPCSyncContext>> BRPCClient::async_Signup(const ReqSignup *req)
{
	using RESULT = std::pair<RespSignup, srpc::RPCSyncContext>;
	auto *pr = new WFPromise<RESULT>();
	auto fr = pr->get_future();
	auto *task = this->create_rpc_client_task<RespSignup>("Signup", srpc::RPCAsyncFutureCallback<RespSignup>);

	task->serialize_input(req);
	task->user_data = pr;
	task->start();
	return fr;
}

inline srpc::BRPCClientTask *BRPCClient::create_Signup_task(SignupDone done)
{
	return this->create_rpc_client_task("Signup", std::move(done));
}

inline TRPCClient::TRPCClient(const char *host, unsigned short port):
	srpc::TRPCClient("UserService")
{
	struct srpc::RPCClientParams params = srpc::RPC_CLIENT_PARAMS_DEFAULT;
	
	params.host = host;
	params.port = port;
	this->srpc::TRPCClient::init(&params);
}

inline TRPCClient::TRPCClient(const struct srpc::RPCClientParams *params):
	srpc::TRPCClient("UserService")
{
	const struct srpc::RPCClientParams *temp = params;
	struct srpc::RPCClientParams temp_params;
	
	this->srpc::TRPCClient::init(temp);
}

inline void TRPCClient::Signup(const ReqSignup *req, SignupDone done)
{
	auto *task = this->create_rpc_client_task("/UserService/Signup", std::move(done));

	if (this->params.callee_timeout >= 0)
		task->get_req()->set_callee_timeout(this->params.callee_timeout);
	if (!this->params.caller.empty())
		task->get_req()->set_caller_name(this->params.caller);
	task->serialize_input(req);
	task->start();
}

inline void TRPCClient::Signup(const ReqSignup *req, RespSignup *resp, srpc::RPCSyncContext *sync_ctx)
{
	auto res = this->async_Signup(req).get();

	if (resp && res.second.success)
		*resp = std::move(res.first);

	if (sync_ctx)
		*sync_ctx = std::move(res.second);
}

inline WFFuture<std::pair<RespSignup, srpc::RPCSyncContext>> TRPCClient::async_Signup(const ReqSignup *req)
{
	using RESULT = std::pair<RespSignup, srpc::RPCSyncContext>;
	auto *pr = new WFPromise<RESULT>();
	auto fr = pr->get_future();
	auto *task = this->create_rpc_client_task<RespSignup>("/UserService/Signup", srpc::RPCAsyncFutureCallback<RespSignup>);

	if (this->params.callee_timeout >= 0)
		task->get_req()->set_callee_timeout(this->params.callee_timeout);
	if (!this->params.caller.empty())
		task->get_req()->set_caller_name(this->params.caller);
	task->serialize_input(req);
	task->user_data = pr;
	task->start();
	return fr;
}

inline srpc::TRPCClientTask *TRPCClient::create_Signup_task(SignupDone done)
{
	auto *task = this->create_rpc_client_task("/UserService/Signup", std::move(done));

	if (!this->params.caller.empty())
		task->get_req()->set_caller_name(this->params.caller);

	return task;
}

inline TRPCHttpClient::TRPCHttpClient(const char *host, unsigned short port):
	srpc::TRPCHttpClient("UserService")
{
	struct srpc::RPCClientParams params = srpc::RPC_CLIENT_PARAMS_DEFAULT;
	
	params.host = host;
	params.port = port;
	this->srpc::TRPCHttpClient::init(&params);
}

inline TRPCHttpClient::TRPCHttpClient(const struct srpc::RPCClientParams *params):
	srpc::TRPCHttpClient("UserService")
{
	const struct srpc::RPCClientParams *temp = params;
	struct srpc::RPCClientParams temp_params;
	
	this->srpc::TRPCHttpClient::init(temp);
}

inline void TRPCHttpClient::Signup(const ReqSignup *req, SignupDone done)
{
	auto *task = this->create_rpc_client_task("Signup", std::move(done));

	if (this->params.callee_timeout >= 0)
		task->get_req()->set_callee_timeout(this->params.callee_timeout);
	if (!this->params.caller.empty())
		task->get_req()->set_caller_name(this->params.caller);
	task->serialize_input(req);
	task->start();
}

inline void TRPCHttpClient::Signup(const ReqSignup *req, RespSignup *resp, srpc::RPCSyncContext *sync_ctx)
{
	auto res = this->async_Signup(req).get();

	if (resp && res.second.success)
		*resp = std::move(res.first);

	if (sync_ctx)
		*sync_ctx = std::move(res.second);
}

inline WFFuture<std::pair<RespSignup, srpc::RPCSyncContext>> TRPCHttpClient::async_Signup(const ReqSignup *req)
{
	using RESULT = std::pair<RespSignup, srpc::RPCSyncContext>;
	auto *pr = new WFPromise<RESULT>();
	auto fr = pr->get_future();
	auto *task = this->create_rpc_client_task<RespSignup>("Signup", srpc::RPCAsyncFutureCallback<RespSignup>);

	if (this->params.callee_timeout >= 0)
		task->get_req()->set_callee_timeout(this->params.callee_timeout);
	if (!this->params.caller.empty())
		task->get_req()->set_caller_name(this->params.caller);
	task->serialize_input(req);
	task->user_data = pr;
	task->start();
	return fr;
}

inline srpc::TRPCHttpClientTask *TRPCHttpClient::create_Signup_task(SignupDone done)
{
	auto *task = this->create_rpc_client_task("Signup", std::move(done));

	if (!this->params.caller.empty())
		task->get_req()->set_caller_name(this->params.caller);

	return task;
}

} // end namespace UserService

