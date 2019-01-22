/*****************************************************
版权所有:北京三海教育科技有限公司
作者：lijian
版本：V0.0.1
时间：2018-09-18
功能：合成服务所有功能，完成合成服务的所有功能

v 0.0.1
2019.01.22 重构合成服务,Http服务接口处添加队列缓存
2019.01.22 Http服务返回值用枚举类型代替
******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/vfs.h>
#include <sys/syscall.h>
#include <dirent.h>
#include <iostream>
#include <map>
#include <list>
#include <pthread.h>
#include <semaphore.h>

#define APIStr "/live/merge"

string  AACSTR = ".aac";
string  H264STR = ".h264";
string  JSONSTR = ".json";
string  MERGESTR = "merge.";
string  updateOnlineUrl;
string  RELATIVEPATH= "/home/record_server/recordFile/";
string  IPPORT= "http://192.168.1.205:8080/live/";
string  serverName = "合成服务105";
string  IpPort = "http://";
string  LOGDIR = "./mergeLog/";

//Http API方法名
string ServerCreate;
string ServerDelete;
string ServerSelect;
string ServerUpdate;

string liveUpdate;
string liveSelect;
string liveUpload;

const char *s_http_port = "8081";
int merge_serverId = 0;  //录制服务ID
int httpSev_flag = 1;
int merge_flag = 1;

typedef pair<int , string> PAIR;

ostream& operator<<(ostream& out, const PAIR& p) 
{
  return out << p.first << "\t" << p.second;
}

//Http服务返回值枚举
enum RESCODE{ 
   NO_ERROR = 0, 
   LIVEID_ERROR,
   METHOD_ERROR 
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
