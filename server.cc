#include "token.hpp"
#include "hash.hpp"
#include "oss.hpp"
#include "amqp.hpp"
#include "user.srpc.h"
#include <iostream>
#include "wfrest/HttpServer.h"
#include "workflow/WFFacilities.h"
#include <json.hpp>
#include <workflow/MySQLResult.h>
#include <ppconsul/ppconsul.h>
#include <sys/stat.h>
#include <sys/types.h>
using namespace wfrest;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using namespace ppconsul;
static WFFacilities::WaitGroup waitGroup(1);

class NetDisk
{
public:
    NetDisk() : _svr(),
                _waitGroup(1) {}
    void start(unsigned short port)
    {
        loadStaticResources();
        loadSignupModule();
        loadSigninModule();
        loadUserInfoModule();
        loadFileUploadModule();
        loadUserFileListModule();
        loadFileDownloadModule();

        if (_svr.track().start(port) == 0)
        {
            _svr.list_routes();
            _waitGroup.wait();
            _svr.stop();
        }
        else
        {
            fprintf(stderr, "Cannot start server");
        }
    }

private:
    void loadStaticResources();
    void loadSignupModule();
    void loadSigninModule();
    void loadUserInfoModule();
    void loadFileUploadModule();
    void loadUserFileListModule();
    void loadFileDownloadModule();

private:
    HttpServer _svr;
    WFFacilities::WaitGroup _waitGroup;
    OssUploader _uploader;
    Publisher _publisher;
};

void NetDisk::loadStaticResources()
{
    // 等待请求的响应
    _svr.GET("/user/signup", [](const HttpReq *req, HttpResp *resp)
             { resp->File("./static/view/signup.html"); });

    _svr.GET("/static/view/signin.html", [](const HttpReq *req, HttpResp *resp)
             { resp->File("./static/view/signin.html"); });

    _svr.GET("/static/view/home.html", [](const HttpReq *req, HttpResp *resp)
             { resp->File("./static/view/home.html"); });

    _svr.GET("/static/img/avatar.jpeg", [](const HttpReq *req, HttpResp *resp)
             { resp->File("./static/img/avatar.jpeg"); });

    _svr.GET("/static/js/auth.js", [](const HttpReq *req, HttpResp *resp)
             { resp->File("./static/js/auth.js"); });

    _svr.GET("/static/view/index.html", [](const HttpReq *req, HttpResp *resp)
             { resp->File("./static/view/index.html"); });

    _svr.GET("/file/upload", [](const HttpReq *, HttpResp *resp)
             { resp->File("./static/view/index.html"); });

    _svr.GET("/file/upload/success", [](const HttpReq *, HttpResp *resp)
             { resp->File("./static/view/home.html"); });
}

void NetDisk::loadSignupModule()
{
    _svr.POST("/user/signup", [](const HttpReq *req, HttpResp *resp, SeriesWork *series)
              {
                     // 解析请求
                     auto &formKV = req->form_kv();
                     string username = formKV["username"];
                     string password = formKV["password"];

                     // TODO
                     // 发起RPC请求
                     //想要注册 就要对注册的程序发去username和password 就要知道程序的ip和
                     //端口 可以通过查询注册中心找到 通过rpc发送请求
                     GOOGLE_PROTOBUF_VERIFY_VERSION;

                     // 使用 Consul 查询服务地址和端口
                     Consul consul("127.0.0.1:8500", ppconsul::kw::dc = "dc1"); // 连接到 Consul
                     agent::Agent agent(consul);//注册中心代理
    
                     // 查询服务信息
                     auto services = agent.services();
                     auto it = services.find("SignupService1");
                     if (it == services.end())
                     {
                         std::cerr << "Service not found in Consul!" << std::endl;
                         return -1;
                     }
                 
                     // 获取服务地址和端口
                     const std::string ip = it->second.address;
                     unsigned short port = it->second.port;
                 
                     std::cout << "Connecting to service at " << ip << ":" << port << std::endl;
                     //client是rpc服务端再客户端的代理
                     UserService::SRPCClient client(ip.c_str(),port);

                     ReqSignup signup_req;
                     // TODO
                     signup_req.set_username(username);
                     signup_req.set_password(password);
                     // 创建RPC任务, 并将其加入到序列中运行
                     //这里的response是rpc服务端的回应 ctx是SRPC 框架中的上下文对象，用于管理和获取当前 RPC 调用的状态和元信息
                     auto rpctask = client.create_Signup_task([resp](RespSignup *response, srpc::RPCContext *ctx)
                    {
                    if(ctx->success() && response->code() == 0) {
                        resp->String("SUCCESS");
                    } else {
                        resp->String("Signup Failed");
                    } });

                     // 3.1 设置任务的属性 设置rpc请求的参数
                     rpctask->serialize_input(&signup_req);
                     // 3.2 交给序列去执行
                     series->push_back(rpctask); });
}

void NetDisk::loadSigninModule()
{
    _svr.POST("/user/signin", [](const HttpReq *req, HttpResp *resp, SeriesWork *series)
              {
        // 解析请求, 获取用户名和密码
        if(req->content_type() == APPLICATION_URLENCODED){
            auto &formKV = req->form_kv();
            string username = formKV["username"];
            string password = formKV["password"];
            cout << "username: " << username << endl;
            cout << "password: " << password << endl;
            // 对密码进行加密
            string salt = "12345678";
            string encodedPasswd = crypt(password.c_str(), "12345678");
            cout << "encodedPasswd: " << encodedPasswd << endl;
            // 访问 MySQL, 进行登录校验
            string url("mysql://root:123456@localhost");
            auto mysqlTask = WFTaskFactory::create_mysql_task(url, 1, [resp, encodedPasswd, username, salt, url, series](WFMySQLTask *task){
                // 对任务进行判断
                using namespace protocol;
                MySQLResultCursor cursor(task->get_resp());
                // 读取结果集
                vector<vector<protocol::MySQLCell>> rows;
                cursor.fetch_all(rows);
                if(rows[0][0].is_string()){
                    string result = rows[0][0].as_string();
                    if(result == encodedPasswd){
                        // 登录成功, 生成Token
                        Token token(username, salt);
                        string strToken = token.genToken();

                        // 将Token写入数据库
                        auto mysqlTask2 = WFTaskFactory::create_mysql_task(url, 1, nullptr);
                        string sql2 = "REPLACE INTO tbl_sql.tbl_user_token(user_name, user_token) VALUES('"
                        + username + "', '" + strToken + "');";
                        cout << "sql2: " << sql2 << endl;
                        mysqlTask2->get_req()->set_query(sql2);
                        series->push_back(mysqlTask2);

                        // 生成响应信息, 发送给客户端
                        Json respMsg;
                        respMsg["data"]["Token"] = strToken;
                        respMsg["data"]["Username"] = username;
                        respMsg["data"]["Location"] = "/static/view/home.html";
                        resp->String(respMsg.dump());
                    }
                    else
                    {
                        // 验证失败
                        printf("password error.\n");
                        resp->String("password error!");
                    }
                }
                else
                {
                    printf("server 500 error.\n");
                    resp->set_status_code("500");
                    resp->set_reason_phrase("Internet Service Error");
                }
                // 测试结果集
                printf("%ld rows in set.\n", rows.size());
            });
            string sql = "select user_pwd from tbl_sql.tbl_user where user_name='";
            sql += username + "' limit 1";
            cout << "sql:\n" << sql << endl;
            mysqlTask->get_req()->set_query(sql);
            series->push_back(mysqlTask);
        } });
}

void NetDisk::loadUserInfoModule()
{
    _svr.GET("/user/info", [](const HttpReq *req, HttpResp *resp)
             {
        //1. 解析请求
        string username = req->query("username");
        string token = req->query("token");
        printf("username: %s\n", username.c_str());
        printf("token: %s\n", token.c_str());
        //2. 校验token
        
        //3. 查询数据库，获取用户的注册时间
        string url("mysql://root:123456@localhost");
        string sql("select signup_at from tbl_sql.tbl_user where user_name = '");
        sql += username + "' limit 1;";
        cout << "sql:\n" << sql << endl;

        using namespace protocol;
        resp->MySQL(url, sql, [resp, username](MySQLResultCursor * pcursor){
            using std::vector;
            vector<vector<MySQLCell>> rows;
            pcursor->fetch_all(rows);

            if(rows[0][0].is_datetime()) {
                Json respMsg;
                respMsg["data"]["Username"] = username;
                respMsg["data"]["SignupAt"] = rows[0][0].as_datetime();
                resp->String(respMsg.dump());
            } else {
                resp->String("User info get failed");
            }
        }); });
}

void NetDisk::loadFileUploadModule()
{
    //
    _svr.POST("/file/upload", [this](const HttpReq *req, HttpResp *resp, SeriesWork *series)
              {
        //1. 解析请求
        if(req->content_type() == MULTIPART_FORM_DATA) {
            auto & form = req->form();
            auto pairKV = form["file"];
            cout << "filename: " <<  pairKV.first << endl;
            /* cout << "content: " << pairKV.second << endl; */
            string filename = form["file"].first;
            string content = form["file"].second;
            //2. 将文件内容写入本地
            mkdir("./tmp", 0755);
            string filepath = "./tmp/" + filename;
            int fd = open(filepath.c_str(), O_CREAT|O_RDWR, 0644);
            if(fd < 0) {
                perror("open");
                resp->String("upload file error");
                return;
            }
            int ret = write(fd, content.c_str(), content.size());
            close(fd);

            /* 初始化OSS账号信息。*/
            // 本地路径
            // std::string FileName = filepath;
            // OSS路径
            // std::string ObjectName = "netdisk/" + filename;
            // _uploader.doUpload(FileName, ObjectName);

            // TODO
            // 将上传的东西放到消息队列
            std::string ObjectName = "netdisk/" + filename;
            Json uploaderInfo;
            uploaderInfo["filePath"] = filepath;
            uploaderInfo["objectName"] = ObjectName;
            _publisher.doPublish(uploaderInfo.dump());

            //3. 重定向到上传成功的页面
            resp->headers["Location"] = "/file/upload/success";
            resp->headers["Content-Type"] = "text/html";
            resp->set_status_code("301");
            resp->set_reason_phrase("Moved Permanently");
            //4. 将文件信息更新到数据库中
            //4.1 用户名
            string username = req->query("username");
            //4.2 文件的hash值
            Hash hash(filepath);
            string filehash = hash.sha1();
            string url("mysql://root:123456@localhost");
            auto mysqlTask = WFTaskFactory::create_mysql_task(url, 1, nullptr);
            string sql = "INSERT INTO tbl_sql.tbl_file(user_name, file_sha1,file_size, file_name, status)VALUES('";
            sql += username + "', '" + filehash + "', " + std::to_string(content.size()) + ",'" + filename + "', 0);";
            cout << "sql:\n";
            cout << sql << endl;
            mysqlTask->get_req()->set_query(sql);
            series->push_back(mysqlTask);
        } });
}

void NetDisk::loadUserFileListModule()
{
    _svr.POST("/file/query", [](const HttpReq *req, HttpResp *resp)
              {
        //1. 解析请求
        string username = req->query("username");
        string token = req->query("token");
        auto & formKV = req->form_kv();
        string limitcnt = formKV["limit"];
        cout << "username:" << username << endl;
        cout << "token:" << token << endl;
        cout << "limitcnt:" << limitcnt << endl;
        //2. 校验token
        //...
        //3. 查询数据库
        string sql("select file_sha1, file_name, file_size");
        sql += " from tbl_sql.tbl_file where user_name = '" + username;
        sql += "' limit " + limitcnt + ";";
        cout << "sql:\n" << sql << endl;
        string url("mysql://root:123456@localhost");
        using namespace protocol;
        resp->MySQL(url, sql, [resp, username](MySQLResultCursor * pcursor){
            using std::vector;
            vector<vector<MySQLCell>> rows;
            pcursor->fetch_all(rows);
            if(rows.size() == 0) return;

            Json arrMsg;
            for(size_t i = 0; i < rows.size(); ++i)  {
                Json row;
                row["FileHash"] = rows[i][0].as_string();
                row["FileName"] = rows[i][1].as_string();
                row["FileSize"] = rows[i][2].as_ulonglong();
                row["UploadAt"] = 0;
                row["LastUpdated"] = 0;
                arrMsg.push_back(row);//将arrMsg当成数组来使用
            }  
            resp->String(arrMsg.dump());
        }); });
}

void NetDisk::loadFileDownloadModule()
{
    _svr.GET("/file/downloadurl", [](const HttpReq *req, HttpResp *resp)
             {
                    // 解析请求
                    auto &queryList = req->query_list();
                    string filename = req->query("filename");
                    cout << "filename: " << filename << endl;
                    string filepath = "tmp/" + filename;

                    string downloadurl = "http://8.146.201.227:8868/" + filepath;
                    cout<<downloadurl<<endl;
                    //resp->headers["Access-Control-Allow-Methods"]="GET";
                    resp->headers["Access-Control-Allow-Origin"]="*";
                    resp->String(downloadurl); });
}

int main()
{
    NetDisk netdisk;
    netdisk.start(8888);

    return 0;
}