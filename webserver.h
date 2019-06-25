/*****************************************************
版权所有:北京三海教育科技有限公司
作者：lijian
版本：V0.0.1
时间：2018-09-18
功能：合成服务所有功能，完成合成服务的所有功能

v 0.0.1
2019.01.22 重构合成服务,Http服务接口处添加队列缓存
2019.01.22 Http服务返回值用枚举类型代替

v 0.0.2
2019.06.25 合成服务增加配置文件
2019.06.25 调试注册服务接口
******************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/vfs.h>
#include <sys/syscall.h>
#include <dirent.h>
#include <iostream>
#include <map>
#include <queue>
#include <list>
#include <pthread.h>
#include <semaphore.h>
#include <string>

#include "glog/logging.h"
#include "mongoose.h"
#include "Base/base.h"
#include "CThreadPool.h"
#include "Base/common.h"
//#include "Base/CommonList.h"

extern string ServerPort, IpPort,ServerCreate,ServerDelete,ServerSelect,ServerUpdate,liveUpdate,liveSelect,liveUpload,
              merge_serverId,LOGDIR,IpPort,serverName,APIStr,updateOnlineUrl;

string HttpAPIStr,ServerName,ServerNameAPIStr, ServerCreateStr;

int httpSev_flag = 1;
int merge_flag = 1;
int threadCount = 0;

typedef pair<int , string> PAIR;

ostream& operator<<(ostream& out, const PAIR& p) 
{
  return out << p.first << "\t" << p.second;
}

//Http服务返回值枚举
enum RESCODE{ 
   NO_ERROR = 0, 
   LIVEID_ERROR,
   METHOD_ERROR,
   MALLOC_ERROR 
};

//定时器任务类型
enum TIMER_TYPE{ 
   UPDATEONLINE = 0,
   CHEDISK
};

//Http接口返回值类型
enum PARSE_TYPE{ 
   GETAPI = 0,
   REGISTONLINE,
   UPDATA
};

using json = nlohmann::json;
using namespace std;

CommonList *MergeParmList;

CThreadPool *threadpool;
pthread_t httpServer_t, httpTime_t, mergeManage_t;
LibcurClient *m_httpclient, *s_httpclient;

//读取配置文件对象
CConfigFileReader config_file("server.conf");

//Http处理请求
void ev_handler(struct mg_connection *nc, int ev, void *ev_data);

//处理合成参数 线程
void *mergeManage_fun(void *data);

//解析Http返回json数据
int parseResdata(string &resdata,  int ret ,PARSE_TYPE m_Type);

//Http服务监听 线程
void *httpServer_fun(void *pdata);

//创建日志文件夹
int CreateLogFileDir(const char *sPathName);

//定时上传状态 线程
void *httpTime_fun(void *pdata);

//停止服务
int stopServer();

//启动服务
int startServer();
